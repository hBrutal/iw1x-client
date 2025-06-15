GSL = {
	source = path.join(dependencies.basePath, "GSL")
}

function GSL.import()
	GSL.includes()
end

function GSL.includes()
	includedirs {
		path.join(GSL.source, "include")
	}
end

function GSL.project()

end

table.insert(dependencies, GSL)