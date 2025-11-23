// Lina - HTML compiler for VirtualDub help system
//
// Copyright (C) 1998-2003 Avery Lee
//
// SPDX-License-Identifier: GPL-2.0-or-later
//

#ifndef f_LINA_DOCUMENT_H
#define f_LINA_DOCUMENT_H

#include <string>
#include <list>
#include <map>

class TreeNode;
class TreeDocument;

bool issafevaluechar(int c);

class TreeAttribute {
public:
	std::string mName, mValue;
	bool mbNoValue;
};

class TreeLocation {
public:
	std::string	mName;
};

class TreeNode {
public:
	TreeDocument *mpDocument;
	TreeLocation *mpLocation;
	int mLineno;
	std::string mName;

	typedef std::list<TreeAttribute> Attributes; 
	Attributes mAttribs;

	typedef std::list<TreeNode *> Children;
	Children mChildren;
	bool mbIsText;
	bool mbIsControl;

public:
	TreeNode *ShallowClone();
	const TreeAttribute *Attrib(const std::string& s) const;
	const TreeNode *ResolvePath(const std::string& path, std::string& name) const;
	const TreeNode *Child(const std::string& s) const;

	bool SupportsCDATA() const;

	static void SetSupportsCDATA(const std::string& tagname, bool supports_cdata);
};

class TreeDocument {
public:
	TreeNode *mpRoot;

	std::list<TreeNode> mNodeHeap;
	std::map<std::string, TreeNode *> mMacros;

	TreeDocument();
	TreeNode *AllocNode();
};

#endif
