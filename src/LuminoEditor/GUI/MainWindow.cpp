﻿
#include <LuminoEngine/UI/UIItemsElement.hpp>
#include <LuminoEngine/UI/UIStyle.hpp>
#include "MainWindow.hpp"

class NavigationBar : public ln::UIItemsControl
{
public:
    static const int ItemSize = 40;

	void addItem(const ln::String& text);

protected:
	//virtual UIControl* generateItem(UIElement* content) override;

private:
};

void NavigationBar::addItem(const ln::String& text)
{

	auto textblock = ln::makeObject<ln::UITextBlock>();
	textblock->setText(text);
    textblock->setTextColor(ln::Color::White);
    textblock->setHorizontalAlignment(ln::HAlignment::Center);
    textblock->setVerticalAlignment(ln::VAlignment::Center);

	auto item = ln::makeObject<ln::UICollectionItem>();
	item->addElement(textblock);
    item->setWidth(ItemSize);
    item->setHeight(ItemSize);
    item->setBorderThickness(ln::Thickness(4, 0, 0, 0));
    item->setBorderColor(ln::UIColors::get(ln::UIColorHues::LightGreen));

	UIItemsControl::addItem(item);
}



MainWindow::MainWindow()
{
}

void MainWindow::onLoaded()
{
	UIMainWindow::onLoaded();

	auto layout1 = ln::makeObject<ln::UIStackLayout2>();
	layout1->setOrientation(ln::Orientation::Horizontal);
	setLayoutPanel(layout1);

	auto sidebar = ln::makeObject<NavigationBar>();
	sidebar->setWidth(40);
	sidebar->setBackgroundColor(ln::Color::Gray);
	sidebar->setHorizontalAlignment(ln::HAlignment::Stretch);
	sidebar->setVerticalAlignment(ln::VAlignment::Stretch);
	addElement(sidebar);


    auto model = ln::makeObject<ln::UIFileSystemCollectionModel>();
    model->setRootPath(u"D:/Proj/LN/Lumino");

    auto treeView = ln::makeObject<ln::UITreeView>();
    treeView->setModel(model);
    treeView->setWidth(200);
    treeView->setHeight(300);
    treeView->setBackgroundColor(ln::UIColors::get(ln::UIColorHues::Grey, 2));
    addElement(treeView);

	sidebar->addItem(u"A");
}
