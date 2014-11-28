class GOContainer
{
	gameObject = null;
	behaviourList = [];

	constructor(gameObjectToStore)
	{
		if(gameObjectToStore)
			gameObject = gameObjectToStore;
	}

	function update(deltaTime)
	{
		foreach(scriptObject in behaviourList)
		    scriptObject.update(deltaTime);
	}
}

this.objectList <- [];
//this.objectList.append(PlayerBehaviour("Dummy"));


this.updateObjects <- function(deltaTime)
{	
	foreach(object in this.objectList)
	    object.update(deltaTime);
}


this.findGameObjectContainer <- function(objToFind)
{
	assert(objToFind != null);
	
	local gameObject = null;
	foreach(object in this.objectList)
	{
		if(objToFind.node == object.gameObject.node)
		{
			gameObject = object;
			break;
		}
	}

	return gameObject;
}

this.getScriptIndex <- function(goContainer, scriptName)
{
	local scriptIndex = -1;
	
	foreach(index, scriptObj in goContainer.behaviourList)
	{
		if(scriptObj.type == scriptName)
		{
			scriptIndex = index;
			break;
		}
	}

	return scriptIndex;
}

this.addScript <- function(gameObject, scriptObj)
{
	local goContainer = findGameObjectContainer(gameObject);

	if(goContainer == null)
	{
		local newContainer = GOContainer(gameObject);
		newContainer.behaviourList.append(scriptObj);
		this.objectList.append(newContainer);
	}
	else
	{
		local scriptIndex = getScriptIndex(goContainer, scriptObj.type);
		if(scriptIndex != -1)
		{
			Log.warning("Removing previous instance of " + scriptObj.type +
						" from " + gameObject.name);
			goContainer.behaviourList.remove(scriptIndex);
		}
		goContainer.behaviourList.append(scriptObj);
	}

	Log.message("PlayerBehaviour script added to " + gameObject.name);
}

this.attachScript <- function(gameObject, scriptName)
{
	try
	{
		assert(gameObject != null);

		local initFunc = dofile(this.scriptPath + scriptName + ".nut", true);

		if(initFunc)
		{
			local scriptObj = initFunc(gameObject);
			
			if(scriptObj)			
				addScript(gameObject, scriptObj);
			else
				Log.error("AttachScript", scriptName + " could not be initialized!");
		}
		else
		{
			Log.error("AttachScript", scriptName + " not found");
		}

	}
	catch(error)
	{
		printStack("AttachScript : " + error);
	}
}

this.removeGameObject <- function(gameObject)
{
	try
	{
		assert(gameObject != null);
		
		local index = -1;
		local foundObject = null;
		
		foreach(currentIndex, object in this.objectList)
		{
			if(objToFind.node == object.gameObject.node)
			{
				index = currentIndex;
				foundObject = object;
				break;
			}
		}

		if(index != -1)
		{
			foundObject.behaviourList.clear();
			this.objectList.remove(index);
		}
		else
		{
			Log.warning("GameObject " + gameObject.name + " not found!");
		}
	}
	catch(error)
	{
		printStack("RemoveGameObject : " + error);
	}
}

this.reloadScript <- function(gameObject, scriptName)
{
	try
	{
		assert(gameObject != null);
		assert(dofile(this.scriptPath + scriptName + ".nut", true));
		
		local goContainer = findGameObjectContainer(gameObject);

		if(goContainer)
		{
			local scriptIndex = getScriptIndex(goContainer, scriptName);
			if(scriptIndex != -1)
			{
				Log.message("Reloading " + scriptName + " for " + gameObject.name + "...");
				goContainer.behaviourList.remove(scriptIndex);
				attachScript(gameObject, scriptName);
			}
			else
			{
				Log.error("ReloadScript", scriptName + " is not attached to "
						  + gameObject.name);
			}
		}
		else
		{
			Log.error("ReloadScript", "No scripts attached to " + gameObject.name);
		}
	}
	catch(error)
	{
		printStack("ReloadScript : " + error);
	}
}

this.printStack <- function(error)
{
	local level = 0;
	local infoString = "";
	while(true)
	{
		local infos = getstackinfos(level);
		if(infos)
		{
			infoString += "func : " + infos.func + " \n" +
			              "src  : " + infos.src  + " \n" +
			              "line : " + infos.line + " \n\n";
			level++;
		}
		else
			break;
	}

	if(error)
		Log.error("VM", error + "\nTRACE : \n" + infoString);
	else
		Log.error("TRACE : \n" + infoString);
}

function debughook(event_type,sourcefile,line,funcname)
{
	local fname=funcname?funcname:"unknown";
	local srcfile=sourcefile?sourcefile:"unknown"
	switch (event_type)
	{
	case 'l': //called every line(that contains some code)
		::print("LINE line [" + line + "] func [" + fname + "]");
		::print("file [" + srcfile + "]\n");
		break;
	case 'c': //called when a function has been called
		::print("LINE line [" + line + "] func [" + fname + "]");
		::print("file [" + srcfile + "]\n");
		break;
	case 'r': //called when a function returns
		::print("LINE line [" + line + "] func [" + fname + "]");
		::print("file [" + srcfile + "]\n");
		break;
	}
}


this.init <- function()
{
	this.scriptPath <- "../content/scripts/";
	enabledebuginfo(true);
	seterrorhandler(this.printStack);
	//setdebughook(debughook);
}

init();
