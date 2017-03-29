/*
 * Copyright 2010-2017. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#include "QLSettings.h"
#include "QuickLaunch.h"
#include "SetupListItem.h"

#include <Application.h>
#include <FindDirectory.h>
#include <File.h>
#include <Message.h>
#include <Path.h>
#include <Screen.h>

#include <stdio.h>


QLSettings::QLSettings()
{
	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) != B_OK)
		return;

	// Defaults
	BScreen* screen = new BScreen(B_MAIN_SCREEN_ID);
	BRect resolution = screen->Frame();
	fMainWindowFrame = BRect(
		resolution.Width() / 2 - 340.0 / 2, resolution.Height() / 2 - 120.0 / 2,
		resolution.Width() / 2 + 340.0 / 2, resolution.Height() / 2 + 120.0 / 2);
	delete screen;
	fSetupWindowFrame = fMainWindowFrame.OffsetByCopy(70.0, 120.0);
	fDeskbar = false;
	fShowVersion = false;
	fShowPath = true;
	fDelay = 0;
	fSaveSearch = false;
	fSearchTerm = "";
	fSingleClick = false;
	fOnTop = false;
	fShowIgnore = false;
	fFavoriteList = new BList();

	path.Append("QuickLaunch_settings");
	BFile file(path.Path(), B_READ_ONLY);

	BMessage settings;

	if (file.InitCheck() == B_OK
		&& settings.Unflatten(&file) == B_OK) {
		BRect frame;
		if (settings.FindRect("main window frame", &frame) == B_OK)
			fMainWindowFrame = frame;

		BRect setupframe;
		if (settings.FindRect("setup window frame", &setupframe) == B_OK)
			fSetupWindowFrame = setupframe;

		int32 deskbar;
		if (settings.FindInt32("deskbar", &deskbar) == B_OK)
			fDeskbar = deskbar;

		int32 version;
		if (settings.FindInt32("show version", &version) == B_OK)
			fShowVersion = version;

		int32 path;
		if (settings.FindInt32("show path", &path) == B_OK)
			fShowPath = path;

		int32 delay;
		if (settings.FindInt32("delay", &delay) == B_OK)
			fDelay = delay;

		int32 savesearch;
		if (settings.FindInt32("savesearch", &savesearch) == B_OK)
			fSaveSearch = savesearch;

		BString searchterm;
		if (settings.FindString("searchterm", &searchterm) == B_OK)
			fSearchTerm = searchterm;

		int32 singleclick;
		if (settings.FindInt32("singleclick", &singleclick) == B_OK)
			fSingleClick = singleclick;

		int32 ontop;
		if (settings.FindInt32("ontop", &ontop) == B_OK)
			fOnTop = ontop;

		int32 ignore;
		if (settings.FindInt32("show ignore", &ignore) == B_OK)
			fShowIgnore = ignore;
	}
}


QLSettings::~QLSettings()
{
	QLApp* app = dynamic_cast<QLApp *> (be_app);

	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) < B_OK)
		return;

	BMessage settings;
	settings.AddRect("main window frame", app->fMainWindow->Frame());
	settings.AddRect("setup window frame",
		app->fSetupWindow->ConvertToScreen(app->fSetupWindow->Bounds()));
	settings.AddInt32("deskbar", fDeskbar);
	settings.AddInt32("show version", fShowVersion);
	settings.AddInt32("show path", fShowPath);
	settings.AddInt32("delay", fDelay);
	settings.AddInt32("savesearch", fSaveSearch);
	settings.AddString("searchterm", fSearchTerm);
	settings.AddInt32("singleclick", fSingleClick);
	settings.AddInt32("ontop", fOnTop);
	settings.AddInt32("show ignore", fShowIgnore);

	for (int32 i = 0; i < app->fSetupWindow->fIgnoreList->CountItems(); i++)
	{
		SetupListItem* item = dynamic_cast<SetupListItem *>
			(app->fSetupWindow->fIgnoreList->ItemAt(i));
		if (!item)
			continue;

		if (item->GetItem())
			settings.AddString("item", item->GetItem());
	}

	for (int32 i = 0; i < fFavoriteList->CountItems(); i++)
	{
		entry_ref* favorite = static_cast<entry_ref *>
			(fFavoriteList->ItemAt(i));

		if (!favorite)
			continue;

		BEntry entry(favorite);
		if (entry.InitCheck() == B_OK) {
			BPath path;
			entry.GetPath(&path);
			settings.AddString("favorite", path.Path());
		}
	}

	path.Append("QuickLaunch_settings");
	BFile file(path.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	if (file.InitCheck() == B_OK)
		settings.Flatten(&file);

	delete fFavoriteList;
}


void
QLSettings::InitLists()
{
	QLApp* app = dynamic_cast<QLApp *> (be_app);

	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) != B_OK)
		return;
	path.Append("QuickLaunch_settings");
	BFile file(path.Path(), B_READ_ONLY);

	BMessage settings;
	if (file.InitCheck() == B_OK && settings.Unflatten(&file) == B_OK) {
		BString itemText;
		int32 i = 0;
		while (settings.FindString("item", i++, &itemText) == B_OK) {
			app->fSetupWindow->fIgnoreList->AddItem(
			new SetupListItem(itemText.String()));
		}
		i = 0;
		while (settings.FindString("favorite", i++, &itemText) == B_OK) {
			entry_ref favorite;
			status_t err = get_ref_for_path(itemText.String(), &favorite);
			if (err == B_OK)
				fFavoriteList->AddItem(new entry_ref(favorite));
		}
	}
}
