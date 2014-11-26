#import "UserPromptUtil.h"
#import <Cocoa/Cocoa.h>

static string gEmptyString("");
static NSArray* gAllowedFileTypes = [NSArray arrayWithObject: @"xml"];

static string stringFromNSString(NSString* aString);
static NSString* NSStringFromString(const string& aString);

string UserPromptUtil::SaveFileName()
{
    NSSavePanel* panel = [NSSavePanel savePanel];
    [panel setTitle: @"Select XML file to save to"];
    [panel setAllowedFileTypes: gAllowedFileTypes];
    [panel setCanCreateDirectories: YES];
    [panel setCanSelectHiddenExtension: YES];
    int ret = [panel runModal];
    return (ret==NSFileHandlingPanelOKButton ? stringFromNSString([panel filename]) : gEmptyString);
}

string UserPromptUtil::LoadFileName()
{
    NSOpenPanel* panel = [NSOpenPanel openPanel];
    [panel setTitle: @"Select XML file to load from"];
    [panel setCanChooseDirectories: NO];
    int ret = [panel runModalForTypes: gAllowedFileTypes];
    return (ret==NSFileHandlingPanelOKButton ? stringFromNSString([panel filename]) : gEmptyString);
}

void UserPromptUtil::UserMessageBox(string text, string caption)
{
    NSString* strText = NSStringFromString(text);
    NSString* strCaption = NSStringFromString(caption);
    NSRunInformationalAlertPanel (strCaption, strText, @"OK", nil, nil);
}

string UserPromptUtil::GetExecutablePath()
{
    NSString* path = [[NSBundle mainBundle] resourcePath];
    return stringFromNSString(path) + "/";
}


void UserPromptUtil::SaveScreenShot(int width, int height)
{
    
}

static string stringFromNSString(NSString* aString)
{
    return std::string([aString UTF8String]);
}


static NSString* NSStringFromString(const string& aString)
{
    return [NSString stringWithCString: aString.c_str() encoding: NSUTF8StringEncoding];
}

