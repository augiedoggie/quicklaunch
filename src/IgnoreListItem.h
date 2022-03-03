/*
 * Copyright 2017. All rights reserved.
 * Distributed under the terms of the MIT license.
 *
 * Author:
 *	Humdinger, humdingerb@gmail.com
 */

#ifndef IgnoreListItem_H
#define IgnoreListItem_H

#include <Entry.h>
#include <File.h>
#include <Font.h>
#include <InterfaceDefs.h>
#include <ListItem.h>
#include <MenuItem.h>
#include <Node.h>
#include <NodeInfo.h>
#include <Path.h>
#include <String.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


class IgnoreListItem : public BListItem {
public:
					IgnoreListItem(BString item);
					~IgnoreListItem();

	virtual void	DrawItem(BView*, BRect, bool);
	
	const BString&	GetItem() const { return fItemString; };
		
private:
	BPath*			fPath;
	BString			fItemString;
	bool			fIsDirectory;
};

#endif // IgnoreListItem_H
