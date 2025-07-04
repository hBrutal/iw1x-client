#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "scheduler.hpp"

#include <utils/concurrency.hpp>
#include <utils/hook.hpp>
#include <utils/thread.hpp>

namespace scheduler
{
	struct task
	{
		std::function<bool()> handler{};
		std::chrono::milliseconds interval{};
		std::chrono::high_resolution_clock::time_point last_call{};
	};

	using task_list = std::vector<task>;

	class task_pipeline
	{
	public:
		void add(task&& task)
		{
			new_callbacks_.access([&task](task_list& tasks)
			{
				tasks.emplace_back(std::move(task));
			});
		}

		void execute()
		{
			callbacks_.access([&](task_list& tasks)
			{
				this->merge_callbacks();

				for (auto i = tasks.begin(); i != tasks.end();)
				{
					const auto now = std::chrono::high_resolution_clock::now();
					const auto diff = now - i->last_call;

					if (diff < i->interval)
					{
						++i;
						continue;
					}

					i->last_call = now;

					const auto res = i->handler();
					if (res == cond_end)
					{
						i = tasks.erase(i);
					}
					else
					{
						++i;
					}
				}
			});
		}

	private:
		utils::concurrency::container<task_list> new_callbacks_;
		utils::concurrency::container<task_list, std::recursive_mutex> callbacks_;

		void merge_callbacks()
		{
			callbacks_.access([&](task_list& tasks)
			{
				new_callbacks_.access([&](task_list& new_tasks)
				{
					tasks.insert(tasks.end(), std::move_iterator<task_list::iterator>(new_tasks.begin()), std::move_iterator<task_list::iterator>(new_tasks.end()));
					new_tasks = {};
				});
			});
		}
	};

	volatile bool kill = false;
	std::thread thread;
	task_pipeline pipelines[pipeline::count];
		
	utils::hook::detour hook_CL_Frame;
	utils::hook::detour hook_SV_Frame;
	utils::hook::detour hook_RE_EndFrame;
	utils::hook::detour hook_CG_DrawActive;
		
	static void execute(const pipeline type)
	{
		assert(type >= 0 && type < pipeline::count);
		pipelines[type].execute();
	}
		
	static void stub_RE_EndFrame(int* frontEndMsec, int* backEndMsec)
	{
		execute(pipeline::renderer);
		hook_RE_EndFrame.invoke(frontEndMsec, backEndMsec);
	}
	static void stub_SV_Frame(int msec)
	{
		execute(pipeline::server);
		hook_SV_Frame.invoke(msec);
	}
	static void stub_CL_Frame(int msec)
	{
		execute(pipeline::client);
		hook_CL_Frame.invoke(msec);
	}
	
	// TODO: Try hook CL_CGameRendering instead
	static void stub_CG_DrawActive(game::stereoFrame_t stereoView)
	{
		hook_CG_DrawActive.invoke(stereoView);
		execute(pipeline::cgame);
	}
	
	void schedule(const std::function<bool()>& callback, const pipeline type, const std::chrono::milliseconds delay)
	{
		assert(type >= 0 && type < pipeline::count);

		task task;
		task.handler = callback;
		task.interval = delay;
		task.last_call = std::chrono::high_resolution_clock::now();

		pipelines[type].add(std::move(task));
	}

	void loop(const std::function<void()>& callback, const pipeline type, const std::chrono::milliseconds delay)
	{
		schedule([callback]()
		{
			callback();
			return cond_continue;
		}, type, delay);
	}

	void once(const std::function<void()>& callback, const pipeline type, const std::chrono::milliseconds delay)
	{
		schedule([callback]()
		{
			callback();
			return cond_end;
		}, type, delay);
	}
	
	class component final : public component_interface
	{
	public:
		void post_start() override
		{
			thread = utils::thread::create_named_thread("Async Scheduler", []()
			{
				while (!kill)
				{
					execute(pipeline::async);
					std::this_thread::sleep_for(10ms);
				}
			});
		}

		void post_unpack() override
		{
			hook_CL_Frame.create(0x00411280, stub_CL_Frame);
			hook_SV_Frame.create(0x0045b1d0, stub_SV_Frame);
			hook_RE_EndFrame.create(0x004de4b0, stub_RE_EndFrame);
		}
		
		void post_cgame() override
		{
			hook_CG_DrawActive.create(ABSOLUTE_CGAME_MP(0x30018940), stub_CG_DrawActive);
		}
		
		void pre_destroy() override
		{
			kill = true;
			if (thread.joinable())
				thread.join();
		}
	};
}

REGISTER_COMPONENT(scheduler::component)