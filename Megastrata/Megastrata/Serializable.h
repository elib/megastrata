#pragma once

#include <utils.h>
class TiXmlElement;

enum XmlError
{
	XML_SUCCESS,
	XML_MISSING_ELEMENTS,
	XML_MISSING_FILE,
	XML_MALFORMED_FILE,
	XML_OUTDATED_VERSION
};

class Serializable
{
public:
	virtual void LoadFromXml(TiXmlElement *currentElement) = 0;
	TiXmlElement* CreateElementAndSave(TiXmlElement *parentElement, string name);

	virtual ~Serializable(void) {}

private:
	virtual void SaveToXml(TiXmlElement *currentElement) = 0;
};