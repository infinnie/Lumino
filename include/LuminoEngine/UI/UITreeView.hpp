﻿#pragma once
#include "UIItemsElement.hpp"

namespace ln {

class UITreeItem
	: public UICollectionItem
{
public:
    virtual void setContent(UIElement* value) override;  // TODO: TreeList ように column が必要かも

    void addChild(UITreeItem* item);

    UITreeView* treeView() const { return m_ownerTreeView; }

protected:
    virtual void onExpanded();
    virtual void onCollapsed();

    // base interface
    virtual void onClick(UIMouseEventArgs* e) override;

    // UIElement interface
    virtual const String& elementName() const  override { static String name = u"UITreeItem"; return name; }
	virtual void onViewModelChanged(UIViewModel* newViewModel, UIViewModel* oldViewModel) override;
	virtual Size measureOverride(UILayoutContext* layoutContext, const Size& constraint) override;
	virtual Size arrangeOverride(UILayoutContext* layoutContext, const Size& finalSize) override;
    virtual void onRoutedEvent(UIEventArgs* e) override;

LN_CONSTRUCT_ACCESS:
	UITreeItem();
	void init();

private:
    void expander_Checked(UIEventArgs* e);
    void expander_Unchecked(UIEventArgs* e);

    UITreeView* m_ownerTreeView;
	Ref<UIToggleButton> m_expanderButton;
    Ref<UIElement> m_headerContent;
    List<Ref<UITreeItem>> m_items;
    Ref<UILayoutPanel2> m_itemsLayout;
	Ref<UICollectionItemViewModel> m_model;

    friend class UITreeView;
};

class UITreeView
    : public UIItemsControl
{
public:
    //void setModel(UICollectionModel* model);

    bool isVirtualize() const { return m_model != nullptr; }

protected:
    virtual void onItemClick(UITreeItem* item, UIMouseEventArgs* e);
    virtual Ref<UITreeItem> onRenderItem(UICollectionItemViewModel* viewModel);

    // base interface
	virtual void onViewModelChanged(UIViewModel* newViewModel, UIViewModel* oldViewModel) override;
    virtual void onSourcePropertyChanged(UINotifyPropertyChangedEventArgs* e) override;
    virtual Size arrangeOverride(UILayoutContext* layoutContext, const Size& finalSize) override;

LN_CONSTRUCT_ACCESS:
    UITreeView();
    void init();

private:
    void addItemInternal(UITreeItem* item);
    void makeChildItems(UITreeItem* item);
    //UITreeItem* makeTreeViewItem();

    Ref<UICollectionModel> m_model;

    friend class UITreeItem;
};





//
//class UITreeItem2
//    : public UICollectionItem
//{
//public:
//    UITreeView* treeView() const { return m_ownerTreeView; }
//
//protected:
//    //virtual void onExpanded();
//    //virtual void onCollapsed();
//
//    //// base interface
//    //virtual void onClick(UIMouseEventArgs* e) override;
//
//    //// UIElement interface
//    //virtual const String& elementName() const  override { static String name = u"UITreeItem"; return name; }
//    //virtual Size measureOverride(UILayoutContext* layoutContext, const Size& constraint) override;
//    //virtual Size arrangeOverride(UILayoutContext* layoutContext, const Size& finalSize) override;
//    //virtual void onRoutedEvent(UIEventArgs* e) override;
//
//LN_CONSTRUCT_ACCESS:
//    UITreeItem2();
//    void init();
//
//private:
//    //void expander_Checked(UIEventArgs* e);
//    //void expander_Unchecked(UIEventArgs* e);
//
//    UITreeView* m_ownerTreeView;
//    Ref<UIToggleButton> m_expanderButton;
//    Ref<UIElement> m_headerContent;
//    List<Ref<UITreeItem>> m_items;
//    Ref<UILayoutPanel> m_itemsLayout;
//    Ref<UICollectionItemModel> m_model;
//
//    friend class UITreeView2;
//};
//
//
//class UITreeView2
//    : public UIControl
//{
//public:
//
//protected:
//    //virtual void onItemClick(UITreeItem* item, UIMouseEventArgs* e);
//    //virtual Ref<UITreeItem> onRenderItem(UICollectionItemModel* viewModel);
//
//    // base interface
//    //virtual void onSourcePropertyChanged(UINotifyPropertyChangedEventArgs* e) override;
//    //virtual Size arrangeOverride(UILayoutContext* layoutContext, const Size& finalSize) override;
//
//LN_CONSTRUCT_ACCESS:
//    UITreeView2();
//    void init();
//
//private:
//
//    friend class UITreeItem2;
//};


} // namespace ln

