class GOContainer
{
	gameObject = null;
	behaviourList = null;
	disabledList  = null;
	
	constructor(gameObjectToStore)
	{
		if(gameObjectToStore)
			gameObject = gameObjectToStore;

		behaviourList = [];
		disabledList  = [];
	}

	function update(deltaTime)
	{
		foreach(scriptObject in behaviourList)
		    scriptObject.update(deltaTime);
	}
}

this.objectList <- [];

this.updateObjects <- function(deltaTime)
{	
	foreach(object in this.objectList)
	    object.update(deltaTime);
}

this.findGameObjectContainer <- function(goIdentifier)
{
	assert(goIdentifier != null);	
	local gameObjectContainer = null;
	local compareNode = false;

	if((typeof goIdentifier) != "string")
		compareNode = true;
	
	foreach(object in this.objectList)
	{
		if(compareNode)
		{
			if(goIdentifier.node == object.gameObject.node)
			{
				gameObjectContainer = object;
				break;
			}
		}
		else
		{
			if(goIdentifier == object.gameObject.name)
			{
				gameObjectContainer = object;
				break;
			}
		}
		
	}

	return gameObjectContainer;
}

this.getScriptType <- function(object)
{
	local className = object.getclass();
	local classAttribs = className.getattributes(null);
	return classAttribs.type;
}

this.getScriptIndex <- function(scriptList, scriptName)
{
	local scriptIndex = -1;
	
	foreach(index, scriptObj in scriptList)
	{
		if(getScriptType(scriptObj) == scriptName)
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
	local typeName = getScriptType(scriptObj);
	
	if(goContainer == null)
	{
		local newContainer = GOContainer(gameObject);
		newContainer.behaviourList.append(scriptObj);
		this.objectList.append(newContainer);
	}
	else
	{
		local scriptIndex = getScriptIndex(goContainer.behaviourList, typeName);
		if(scriptIndex != -1)
		{
			Log.warning("Removing previous instance of " + typeName +
						" from " + gameObject.name);
			goContainer.behaviourList.remove(scriptIndex);
			goContainer.behaviourList.append(scriptObj);
		}
		else
		{
			scriptIndex = getScriptIndex(goContainer.disabledList, typeName);
			if(scriptIndex != -1)
			{
				Log.warning("Removing previous instance of " + typeName +
						" from " + gameObject.name);
				goContainer.disabledList.remove(scriptIndex);
				goContainer.disabledList.append(scriptObj);
			}
			else
			{
				goContainer.behaviourList.append(scriptObj);
			}
		}
	}

	Log.message(typeName + " script added to " + gameObject.name);
}

this.attachScript <- function(gameObject, scriptName)
{
	try
	{
		assert(gameObject != null);
		assert(dofile(this.scriptPath + scriptName + ".nut", true));
		
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

this.getScript <- function(goIdentifier, scriptName)
{
	try
	{
		assert(goIdentifier != null);
		local goContainer = findGameObjectContainer(goIdentifier);

		if(goContainer)
		{
			local scriptIndex = getScriptIndex(goContainer.behaviourList, scriptName);
			
			if(scriptIndex != -1)
				return goContainer.behaviourList[scriptIndex];
			else
				Log.error("GetScript", scriptName + " not found on " + goContainer.gameObject.name);
		}
		else
		{
			if((typeof goIdentifier) != "string")
				Log.error("GetScript", "No scripts attached to " + goIdentifier.name);
			else
				Log.error("GetScript", "No scripts attached to " + goIdentifier);
		}
	}
	catch(error)
	{
		printStack("GetScript : " + error);
	}
}

this.processCollision <- function(gameObject, collisionData)
{
	try
	{
		assert(gameObject != null);
		
		local goContainer = findGameObjectContainer(gameObject);

		if(goContainer)
		{
			foreach(script in goContainer.behaviourList)
			    script.onCollision(collisionData);
		}
		// else
		// {
		// 	Log.warning("ProcessCollision : No scripts attached to " + gameObject.name);
		// }
	}
	catch(error)
	{
		printStack("ProcessCollision : " + error);
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
			if(gameObject.node == object.gameObject.node)
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
			Log.message("ScriptManager : Removed " + gameObject.name);
		}
	}
	catch(error)
	{
		printStack("RemoveGameObject : " + error);
	}
}

this.enableScript <- function(goIdentifier, scriptName)
{
	try
	{
		assert(goIdentifier != null);
		local goContainer = findGameObjectContainer(goIdentifier);

		if(goContainer)
		{
			local scriptIndex = getScriptIndex(goContainer.disabledList, scriptName);
			if(scriptIndex != -1)
			{
				local disabledScript = goContainer.disabledList[scriptIndex];
				goContainer.behaviourList.append(disabledScript);
				goContainer.disabledList.remove(scriptIndex);
				Log.message(scriptName + " enabled for " + goContainer.gameObject.name);
			}
			else
			{
				Log.error("EnableScript", scriptName + " is not disabled on " + goContainer.gameObject.name);
			}
		}
		else
		{
			if((typeof goIdentifier) != "string")
				Log.error("EnableScript", "No scripts attached to " + goIdentifier.name);
			else
				Log.error("EnableScript", "No scripts attached to " + goIdentifier);
		}
	}
	catch(error)
	{
		printStack("EnableScript : " + error);
	}
}

this.removeScript <- function(goIdentifier, scriptName)
{
	try
	{
		assert(goIdentifier != null);
		local goContainer = findGameObjectContainer(goIdentifier);

		if(goContainer)
		{
			local scriptIndex = getScriptIndex(goContainer.behaviourList, scriptName);
			if(scriptIndex != -1)
			{
				goContainer.behaviourList.remove(scriptIndex);
				Log.message(scriptName + " removed from " + goContainer.gameObject.name);
			}
			else
			{
				scriptIndex = getScriptIndex(goContainer.disabledList, scriptName);
				if(scriptIndex != -1)
				{
					goContainer.disabledList.remove(scriptIndex);
					Log.message(scriptName + " removed from " + goContainer.gameObject.name);
				}
				else
					Log.error("RemoveScript", scriptName + " is not attached to " + goContainer.gameObject.name);
			}
		}
		else
		{
			if((typeof goIdentifier) != "string")
				Log.error("RemoveScript", "No scripts attached to " + goIdentifier.name);
			else
				Log.error("RemoveScript", "No scripts attached to " + goIdentifier);
		}
	}
	catch(error)
	{
		printStack("RemoveScript : " + error);
	}
}

this.disableScript <- function(goIdentifier, scriptName)
{
	try
	{
		assert(goIdentifier != null);
		local goContainer = findGameObjectContainer(goIdentifier);

		if(goContainer)
		{
			local scriptIndex = getScriptIndex(goContainer.behaviourList, scriptName);
			if(scriptIndex != -1)
			{
				local script = goContainer.behaviourList[scriptIndex];
				goContainer.disabledList.append(script);
				goContainer.behaviourList.remove(scriptIndex);
				Log.message(scriptName + " disabled for " + goContainer.gameObject.name);
			}
			else
			{
				Log.error("DisableScript", scriptName + " is not attached to "
						  + goContainer.gameObject.name);
			}
		}
		else
		{
			if((typeof goIdentifier) != "string")
				Log.error("DisableScript", "No scripts attached to " + goIdentifier.name);
			else
				Log.error("DisableScript", "No scripts attached to " + goIdentifier);
		}
	}
	catch(error)
	{
		printStack("DisableScript : " + error);
	}
}

this.reloadScript <- function(goIdentifier, scriptName)
{
	try
	{
		assert(goIdentifier != null);
		assert(dofile(this.scriptPath + scriptName + ".nut", true));
		local goContainer = findGameObjectContainer(goIdentifier);

		if(goContainer)
		{
			local scriptIndex = getScriptIndex(goContainer.behaviourList, scriptName);
			if(scriptIndex != -1)
			{
				Log.message("Reloading " + scriptName + " for " + goContainer.gameObject.name);
				goContainer.behaviourList.remove(scriptIndex);
				attachScript(goContainer.gameObject, scriptName);
			}
			else
			{
				Log.error("ReloadScript", scriptName + " is not attached to "
						  + goContainer.gameObject.name);
			}
		}
		else
		{
			if((typeof goIdentifier) != "string")
				Log.error("ReloadScript", "No scripts attached to " + goIdentifier.name);
			else
				Log.error("ReloadScript", "No scripts attached to " + goIdentifier);
		}
	}
	catch(error)
	{
		printStack("ReloadScript : " + error);
	}
}

this.reloadScriptType <- function(scriptName)
{
	foreach(goContainer in this.objectList)
	{
		foreach(script in  goContainer.behaviourList)
		{
			if(scriptName == getScriptType(script))
			{
				reloadScript(goContainer.gameObject, scriptName);
				break;
			}
		}
	}
}

this.reloadAllScripts <- function(goIdentifier)
{
	try
	{
		assert(goIdentifier != null);
		local goContainer = findGameObjectContainer(goIdentifier);

		if(goContainer != null)
		{
			//get attached scripts in array
			local attachedScripts = [];
			foreach(script in goContainer.behaviourList)
			{
				local typeName = getScriptType(script);
				attachedScripts.append(typeName);
			}

			//reload each script
			foreach(scriptType in attachedScripts)
     			reloadScript(goContainer.gameObject, scriptType);
		}
		else
		{
			if((typeof goIdentifier) != "string")
				Log.error("ReloadAllScripts", goIdentifier.name + " not registered to ScriptEngine");
			else
				Log.error("ReloadAllScripts", goIdentifier + " not registered to ScriptEngine")
		}
	}
	catch(error)
	{
		printStack("ReloadAllScripts : " + error);
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
