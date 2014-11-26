
#include "TinyXml.h"
#include "UserPromptUtil.h"

#include "Serializable.h"
#include "SubGenerator.h"
#include "Generator.h"
#include "RenderableFactory.h"
#include "Camera.h"
#include "Transform.h"
#include "Vector3d.h"
#include "World.h"
#include "GuiManager.h"

#include <VersionInfo.h>

TiXmlElement* Serializable::CreateElementAndSave(TiXmlElement *parentElement, string name)
{
	TiXmlElement * currentElement = new TiXmlElement( name.c_str() );
	parentElement->LinkEndChild( currentElement );

	//perform the specific save
	this->SaveToXml(currentElement);

	return currentElement;
}

void GuiManager::SaveToXml()
{
	string path = UserPromptUtil::SaveFileName();
	if(path.length() == 0)
		return;
	
	TiXmlDocument doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
	doc.LinkEndChild( decl );  

	TiXmlElement * root = new TiXmlElement( "NoiseViewer" );
	doc.LinkEndChild( root );

	root->SetAttribute("MajorVersion", MAJOR_VERSION);
	root->SetAttribute("MinorVersion", MINOR_VERSION);
	
	m_World.CreateElementAndSave(root, "World");

	m_LayerManager.CreateElementAndSave(root, "LayerManager");

	//ensure adding ".xml" to end!
	int len = path.length();
	string end = path.substr(len - 4, 4);
	if(end.compare(".xml") != 0)
		path.append(".xml");
	doc.SaveFile(path.c_str());
}

XmlError GuiManager::LoadFromXmlWithErrors(string path)
{
	//check length - decide whether to bother user or not
	if(path.length() == 0)
		path = UserPromptUtil::LoadFileName();

	//check length again - see if file is cool.
	if(path.length() == 0)
		return XML_MISSING_FILE;

	TiXmlDocument doc( path.c_str());
	doc.LoadFile();

	if(doc.Error())
		return XML_MALFORMED_FILE;
	
	TiXmlElement * root = doc.FirstChildElement( "NoiseViewer" );
	if(!root) return XML_MISSING_ELEMENTS;

	int major = 0, minor = 0;
	root->QueryIntAttribute("MajorVersion", &major);
	root->QueryIntAttribute("MinorVersion", &minor);
	if(major == 0 || minor == 0) //older than 1.82
	{
		major = 1;
		minor = 81;
	}

	if (major < MAJOR_VERSION)
	{
		UserPromptUtil::UserMessageBox("This XML file was saved by a very old version of Megastrata. Aborting operation.", "Old XML");
		return XML_OUTDATED_VERSION;
	}
	if (major > MAJOR_VERSION || minor > MINOR_VERSION)
	{
		UserPromptUtil::UserMessageBox("This XML file was saved by a newer version of Megastrata. Please upgrade.", "More recent version available");
	}

	if(minor < MINOR_VERSION)
	{
		UserPromptUtil::UserMessageBox("This XML file was saved by an older version of Megastrata. Compatibility not guaranteed.", "Older version");
	}
	
	TiXmlElement * world = root->FirstChildElement( "World" );
	if(!world) return XML_MISSING_ELEMENTS;
	m_World.LoadFromXml(world);

	TiXmlElement * layers = root->FirstChildElement( "LayerManager" );
	if(!layers) return XML_MISSING_ELEMENTS;
	m_LayerManager.LoadFromXml(layers);

	SyncronizeLayerList();

	return XML_SUCCESS;
}

void GuiManager::LoadFromXml()
{
	XmlError result = LoadFromXmlWithErrors();
	if(result != XML_SUCCESS)
	{
		string message;
		switch(result)
		{
		case XML_MISSING_ELEMENTS:
			message = "One or more XML elements are missing.";
			break;
		case XML_MISSING_FILE:
			message = "Could not find specified XML file.";
			break;
		case XML_MALFORMED_FILE:
			message = "File is malformed.";
			break;
		case XML_OUTDATED_VERSION:
			message = "The XML file was created by too old a version of Megastrata.";
			break;
		default:
			message = "Could not read XML file.";
			break;
		}

		UserPromptUtil::UserMessageBox("XML reading failed: " + message, "XML Failure");
	}
}


void SubGenerator::SaveToXml(TiXmlElement *currentElement)
{
	currentElement->SetAttribute("inverse", (int)m_inverse);
	m_transform.CreateElementAndSave(currentElement, "Transform");
	GeneratorBase::SaveToXml(currentElement);
}

void SubGenerator::LoadFromXml(TiXmlElement *currentElement)
{
	currentElement->QueryIntAttribute("inverse", (int*) &m_inverse);
	TiXmlElement * transform = currentElement->FirstChildElement( "Transform" );
	if(!transform) return;

	m_transform.LoadFromXml(transform);
	GeneratorBase::LoadFromXml(currentElement);
}

void Generator::SaveToXml(TiXmlElement *currentElement)
{
	GeneratorBase::SaveToXml(currentElement);
}

void Generator::LoadFromXml(TiXmlElement *currentElement)
{
	GeneratorBase::LoadFromXml(currentElement);
}



void GeneratorBase::SaveToXml(TiXmlElement *currentElement)
{
	currentElement->SetAttribute("TypeID", m_typeID);

	TiXmlElement *variables = new TiXmlElement("Variables");
	currentElement->LinkEndChild(variables);
	for(int i = 0; i < m_variableCount; i++)
	{
		m_layerVariables[i].CreateElementAndSave(variables, "Variable");
	}
}

void GeneratorBase::LoadFromXml(TiXmlElement *currentElement)
{


	TiXmlElement * variables = currentElement->FirstChildElement( "Variables" );
	if(!variables) return;

	TiXmlElement *child = 0;
	int runningIndex = 0;
	while( child = (TiXmlElement*)variables->IterateChildren( child ) )
	{
		//any child here is actually a variable.
		if(runningIndex >= m_variableCount)
			return; //you have an older version of Megastrata than this xml.

		m_layerVariables[runningIndex].LoadFromXml(child);
		runningIndex++;
	}
}

void LayerVariable::SaveToXml(TiXmlElement *currentElement)
{
	//name is just for human-readability of XML
	currentElement->SetAttribute("name", name);
	currentElement->SetDoubleAttribute("OverrideValue", m_overrideValue);

	//put renderable here:
	if(renderable_value != NULL)
		renderable_value->CreateElementAndSave(currentElement, "SubLayer");

}

void LayerVariable::LoadFromXml(TiXmlElement *currentElement)
{
	currentElement->QueryFloatAttribute("OverrideValue", &m_overrideValue);

	//put renderable here:
	TiXmlElement *renderableEl = currentElement->FirstChildElement("SubLayer");
	if(renderableEl != NULL)
	{
		int type_id;
		renderableEl->QueryIntAttribute("TypeID", &type_id);
		SubGenerator* newInput = RenderableFactory::CreateLayer2d(type_id);
		newInput->LoadFromXml(renderableEl);

		this->SetRenderable(newInput);
	}
}

void Camera::SaveToXml(TiXmlElement *currentElement)
{
	Position.CreateElementAndSave(currentElement, "Position");
	Lookat.CreateElementAndSave(currentElement, "Lookat");
}

void Camera::LoadFromXml(TiXmlElement *currentElement)
{
	TiXmlElement* pos = currentElement->FirstChildElement("Position");
	if(!pos) return;

	TiXmlElement* look = currentElement->FirstChildElement("Lookat");
	if(!look) return;

	Position.LoadFromXml(pos);
	Lookat.LoadFromXml(look);
}

void Character::SaveToXml(TiXmlElement *currentElement)
{
	currentElement->SetDoubleAttribute("TurnAngle", m_turnangle);
	currentElement->SetDoubleAttribute("UpAngle", m_upangle);	
	Camera::SaveToXml(currentElement);
}

void Character::LoadFromXml(TiXmlElement *currentElement)
{

	Camera::LoadFromXml(currentElement);
	currentElement->QueryFloatAttribute("TurnAngle", &m_turnangle);
	currentElement->QueryFloatAttribute("UpAngle", &m_upangle);

	SetLookat();
}

void Transform::SaveToXml(TiXmlElement *currentElement)
{
	currentElement->SetDoubleAttribute("scale_x", Scale.x);
	currentElement->SetDoubleAttribute("scale_y", Scale.y);
	currentElement->SetDoubleAttribute("translate_x", Translation.x);
	currentElement->SetDoubleAttribute("translate_y", Translation.y);
}

void Transform::LoadFromXml(TiXmlElement *currentElement)
{
	currentElement->QueryFloatAttribute("scale_x", &Scale.x);
	currentElement->QueryFloatAttribute("scale_y", &Scale.y);
	currentElement->QueryFloatAttribute("translate_x", &Translation.x);
	currentElement->QueryFloatAttribute("translate_y", &Translation.y);
}

void Vector3d::SaveToXml(TiXmlElement *currentElement)
{
	currentElement->SetDoubleAttribute("x", x);
	currentElement->SetDoubleAttribute("y", y);
	currentElement->SetDoubleAttribute("z", z);
}

void Vector3d::LoadFromXml(TiXmlElement *currentElement)
{
	currentElement->QueryFloatAttribute("x", &x);
	currentElement->QueryFloatAttribute("y", &y);
	currentElement->QueryFloatAttribute("z", &z);
}

void World::SaveToXml(TiXmlElement *currentElement)
{
	camera.CreateElementAndSave(currentElement, "Camera");
	character.CreateElementAndSave(currentElement, "Character");
	currentElement->SetAttribute("VisualMode", (int)m_visualmode);
	currentElement->SetAttribute("RenderMode", (int)m_rendermode);
	
	currentElement->SetAttribute("WindowSizeX", (int)m_architecture.m_WindowSizeX);
	currentElement->SetAttribute("WindowSizeY", (int)m_architecture.m_WindowSizeY);
}

void World::LoadFromXml(TiXmlElement *currentElement)
{
	TiXmlElement * cam = currentElement->FirstChildElement( "Camera" );
	if(cam)
		camera.LoadFromXml(cam);

	TiXmlElement * chr = currentElement->FirstChildElement( "Character" );
	if(chr)
		character.LoadFromXml(chr);

	currentElement->QueryIntAttribute("VisualMode", (int*)&m_visualmode);
	currentElement->QueryIntAttribute("RenderMode", (int*)&m_rendermode);

	currentElement->QueryIntAttribute("WindowSizeX", (int*)&m_architecture.m_WindowSizeX);
	currentElement->QueryIntAttribute("WindowSizeY", (int*)&m_architecture.m_WindowSizeY);
}

void LayerManager::SaveToXml(TiXmlElement *currentElement)
{
	for(int i = 0; i < m_architectureCount; i++)
	{
		TiXmlElement* archel = m_architecture[i]->CreateElementAndSave(currentElement, "Generator");
	}
}

void LayerManager::LoadFromXml(TiXmlElement *currentElement)
{
	TiXmlElement *child = 0;
	while( child = (TiXmlElement*)currentElement->IterateChildren( child ) )
	{
		//any child here is actually a piece of architecture.
		int type_id;
		child->QueryIntAttribute("TypeID", &type_id);
		Generator* newLayer = RenderableFactory::CreateGenerator(type_id);
		newLayer->LoadFromXml(child);
		m_architecture[m_architectureCount] = newLayer;

		m_architectureCount++;
	}
}