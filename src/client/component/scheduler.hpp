#pragma once

namespace scheduler
{
	enum pipeline
	{
		async = 0,	// Asynchronuous pipeline, disconnected from the game
		renderer,	// The game's rendering pipeline
		server,		// The game's server thread
		main,		// The game's main thread
		count,
	};

	static const bool cond_continue = false;
	static const bool cond_end = true;

	void schedule(const std::function<bool()>& callback, pipeline type = pipeline::async, std::chrono::milliseconds delay = 0ms);
	void loop(const std::function<void()>& callback, pipeline type = pipeline::async, std::chrono::milliseconds delay = 0ms);
	void once(const std::function<void()>& callback, pipeline type = pipeline::async, std::chrono::milliseconds delay = 0ms);
}