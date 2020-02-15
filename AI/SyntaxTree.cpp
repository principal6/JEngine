#include "SyntaxTree.h"
#include <cassert>

using std::vector;
using std::string;
using std::swap;

CSyntaxTree::CSyntaxTree()
{
}

CSyntaxTree::~CSyntaxTree()
{
	Destroy();
}

void CSyntaxTree::MoveChildrenAsHead(SSyntaxTreeNode*& Src, SSyntaxTreeNode*& Dest)
{
	if (!Src || !Dest) return;
	if (Src->vChildNodes.empty()) return;

	vector<SSyntaxTreeNode*> ToBackup{ Dest->vChildNodes };
	Dest->vChildNodes.clear();

	for (auto& FromChild : Src->vChildNodes)
	{
		FromChild->ParentNode = Dest;
	}
	Dest->vChildNodes.assign(Src->vChildNodes.begin(), Src->vChildNodes.end());

	Src->vChildNodes.clear();

	for (auto& ToBackupChild : ToBackup)
	{
		Dest->vChildNodes.emplace_back(ToBackupChild);
	}
	ToBackup.clear();
}

void CSyntaxTree::MoveChildrenAsTail(SSyntaxTreeNode*& Src, SSyntaxTreeNode*& Dest)
{
	if (!Src || !Dest) return;
	if (Src->vChildNodes.empty()) return;

	for (auto& SrcChild : Src->vChildNodes)
	{
		SrcChild->ParentNode = Dest; // @important
		Dest->vChildNodes.emplace_back(SrcChild);
	}
	Src->vChildNodes.clear();
}

void CSyntaxTree::MoveAsHead(SSyntaxTreeNode*& Src, SSyntaxTreeNode*& NewParent)
{
	if (!Src || !NewParent) return;
	if (!Src->ParentNode) return;

	SSyntaxTreeNode* SrcParentCopy{ Src->ParentNode };

	bool bFound{ false };
	size_t iSrcNode{};
	for (iSrcNode = 0; iSrcNode < SrcParentCopy->vChildNodes.size(); ++iSrcNode)
	{
		if (SrcParentCopy->vChildNodes[iSrcNode] == Src)
		{
			bFound = true;
			break;
		}
	}
	assert(bFound);

	Src->ParentNode = NewParent;
	NewParent->vChildNodes.insert(NewParent->vChildNodes.begin(), Src);

	SrcParentCopy->vChildNodes.erase(SrcParentCopy->vChildNodes.begin() + iSrcNode);
}

void CSyntaxTree::MoveAsTail(SSyntaxTreeNode* Src, SSyntaxTreeNode* NewParent)
{
	if (!Src || !NewParent) return;
	if (!Src->ParentNode) return;

	bool bFound{ false };
	size_t iSrcNode{};
	for (iSrcNode = 0; iSrcNode < Src->ParentNode->vChildNodes.size(); ++iSrcNode)
	{
		if (Src->ParentNode->vChildNodes[iSrcNode] == Src)
		{
			bFound = true;
			break;
		}
	}
	assert(bFound);

	Src->ParentNode->vChildNodes.erase(Src->ParentNode->vChildNodes.begin() + iSrcNode);
	
	Src->ParentNode = NewParent;
	NewParent->vChildNodes.emplace_back(Src);
}

// Parent by Child
void CSyntaxTree::SubstituteParentByChild(SSyntaxTreeNode*& Node, size_t ChildIndex)
{
	if (!Node || (ChildIndex >= Node->vChildNodes.size())) return;

	SSyntaxTreeNode* ChildCopy{ Node->vChildNodes[ChildIndex] };
	Node->vChildNodes.clear();
	ChildCopy->ParentNode = Node->ParentNode;

	bool bFound{ false };
	size_t iNode{};
	for (iNode = 0; iNode < Node->ParentNode->vChildNodes.size(); ++iNode)
	{
		if (Node->ParentNode->vChildNodes[iNode] == Node)
		{
			bFound = true;
			break;
		}
	}
	assert(bFound);

	delete Node;

	Node = ChildCopy;
}

void CSyntaxTree::Substitute(const SSyntaxTreeNode& NewNode, SSyntaxTreeNode*& Dest)
{
	if (!Dest) return;

	Dest->eType = NewNode.eType;
	Dest->Identifier = NewNode.Identifier;
	
	for (auto& DestChild : Dest->vChildNodes)
	{
		delete DestChild;
		DestChild = nullptr;
	}
	Dest->vChildNodes.clear();
}

void CSyntaxTree::Remove(SSyntaxTreeNode*& Node)
{
	if (!Node) return;

	SSyntaxTreeNode* ParentCopy{ Node->ParentNode };
	bool bFound{ false };
	size_t iNode{};
	for (iNode = 0; iNode < ParentCopy->vChildNodes.size(); ++iNode)
	{
		if (ParentCopy->vChildNodes[iNode] == Node)
		{
			bFound = true;
			break;
		}
	}
	assert(bFound);

	delete Node;
	Node = nullptr;
	ParentCopy->vChildNodes.erase(ParentCopy->vChildNodes.begin() + iNode);
}

void CSyntaxTree::Create(const SSyntaxTreeNode& RootNode)
{
	Destroy();

	m_SyntaxTreeRoot = new SSyntaxTreeNode(RootNode);
	m_PtrCurrentNode = m_SyntaxTreeRoot;
}

void CSyntaxTree::CopyFrom(const SSyntaxTreeNode* const Node)
{
	Destroy();

	_CopyFrom(m_SyntaxTreeRoot, nullptr, Node);
}

void CSyntaxTree::Destroy()
{
	if (m_SyntaxTreeRoot)
	{
		delete m_SyntaxTreeRoot;
		m_SyntaxTreeRoot = nullptr;
	}
}

void CSyntaxTree::_CopyFrom(SSyntaxTreeNode*& DestNode, SSyntaxTreeNode* const DestParentNode, const SSyntaxTreeNode* const SrcNode)
{
	if (!SrcNode) return;

	assert(!DestNode);

	DestNode = new SSyntaxTreeNode(*SrcNode);
	DestNode->ParentNode = DestParentNode;
	DestNode->vChildNodes.clear(); // @important

	for (const auto& SrcChild : SrcNode->vChildNodes)
	{
		DestNode->vChildNodes.emplace_back();

		_CopyFrom(DestNode->vChildNodes.back(), DestNode, SrcChild);
	}
}

void CSyntaxTree::InsertChild(const SSyntaxTreeNode& Content)
{
	SSyntaxTreeNode* NewNode{ new SSyntaxTreeNode(Content) };
	NewNode->ParentNode = m_PtrCurrentNode; // @important

	m_PtrCurrentNode->vChildNodes.emplace_back(NewNode);
}

void CSyntaxTree::GoToLastChild()
{
	if (m_PtrCurrentNode && m_PtrCurrentNode->vChildNodes.size())
	{
		m_PtrCurrentNode = m_PtrCurrentNode->vChildNodes.back();
	}
}

void CSyntaxTree::GoToParent()
{
	if (m_PtrCurrentNode && m_PtrCurrentNode->ParentNode) // @important: root node must be unique
	{
		m_PtrCurrentNode = m_PtrCurrentNode->ParentNode;
	}
}

bool CSyntaxTree::IsCreated() const
{
	return ((m_SyntaxTreeRoot) ? true : false);
}

const std::string& CSyntaxTree::Serialize()
{
	m_SerializedTree.clear();
	SerializeTree(m_SyntaxTreeRoot, 0);
	return m_SerializedTree;
}

void CSyntaxTree::SerializeTree(SSyntaxTreeNode* const CurrentNode, size_t Depth)
{
	if (CurrentNode)
	{
		if (CurrentNode == m_SyntaxTreeRoot) m_SerializedTree.clear();

		if (Depth > 0)
		{
			if (Depth == 1)
			{
				m_SerializedTree += "+-- ";
			}
			else
			{
				for (size_t iDepth = 0; iDepth < Depth - 1; ++iDepth)
				{
					m_SerializedTree += "    ";
				}
				m_SerializedTree += "+-- ";
			}
		}
		m_SerializedTree += CurrentNode->Identifier;

		switch (CurrentNode->eType)
		{
		case SSyntaxTreeNode::EType::Directive:
			m_SerializedTree += "     *DIR";
			break;
		case SSyntaxTreeNode::EType::Operator:
			m_SerializedTree += "     *OPR";
			break;
		case SSyntaxTreeNode::EType::Literal:
			m_SerializedTree += "     *LIT";
			break;
		case SSyntaxTreeNode::EType::Identifier:
			m_SerializedTree += "     *IDN";
			break;
		case SSyntaxTreeNode::EType::Grouping:
			m_SerializedTree += "     *GRP";
			break;
		default:
			break;
		}

		m_SerializedTree += '\n';

		for (const auto& ChildNode : CurrentNode->vChildNodes)
		{
			SerializeTree(ChildNode, Depth + 1);
		}
	}
}

SSyntaxTreeNode*& CSyntaxTree::GetRootNode()
{
	return m_SyntaxTreeRoot;
}
