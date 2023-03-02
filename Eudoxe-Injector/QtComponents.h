#pragma once


#pragma once

#include "Includes.h"

#define CONCACT_MACRO_CALL(c,_c) c  _c

#define  UI_RGB(r,g,b) "rgb("#r","#g","#b");\n"

#define  UI_RGB_ALPHA(r,g,b,a) "rgba(" #r "," #g "," #b "," #a ");\n"

#define TEXT_COLOR(color) ((QString)(CONCACT_MACRO_CALL("color : ", color)))

#define UI_MARGIN(top,right,bottom,left) "margin : " #top "px " #right "px " #bottom "px " #left "px;\n"

#define UI_TOP_MARGIN(top)       ((QString)("margin-top     : "  #top    "px;\n"))
#define UI_RIGHT_MARGIN(right)   ((QString)("margin-right   : "  #right  "px;\n"))
#define UI_BOTTOM_MARGIN(bottom) ((QString)("margin-bottom  : "  #bottom "px;\n"))
#define UI_LEFT_MARGIN(left)     ((QString)("margin-left    : "  #left   "px;\n"))

#define UI_PADDING(top,right,bottom,left) ((QString)("padding : " #top "px " #right "px " #bottom "px " #left "px;\n"))

#define UI_TOP_PADDING(top)       ((QString)("padding-top     : "  #top    "px;\n"))
#define UI_RIGHT_PADDING(right)   ((QString)("padding-right   : "  #right  "px;\n"))
#define UI_BOTTOM_PADDING(bottom) ((QString)("padding-bottom  : "  #bottom "px;\n"))
#define UI_LEFT_PADDING(left)     ((QString)("padding-left    : "  #left   "px;\n"))


#define UI_BACKGROUND_COLOR(color)  ((QString)(CONCACT_MACRO_CALL("background-color : ", color)))

#define LETTER_SPACING(spacing) ((QString)("letter-spacing : "  #spacing "px;\n"))


#define BORDER(width,color) ((QString)(CONCACT_MACRO_CALL("border : " #width "px solid ", color)))

#define BORDER_TOP(width,color) (((QString) CONCACT_MACRO_CALL("border-top       : " #width "px solid ", color)))
#define BORDER_RIGHT(width,color) (((QString)CONCACT_MACRO_CALL("border-right   : " #width "px solid ", color)))
#define BORDER_BOTTOM(width,color) (((QString)CONCACT_MACRO_CALL("border-bottom : " #width "px solid ", color)))
#define BORDER_LEFT(width,color) (((QString) CONCACT_MACRO_CALL("border-left     : " #width "px solid ", color)))

#define BORDER_NULL ((QString)"border : none;border-top:none;border-right:none;border-bottom:none;border-left:none;")


#define BORDER_RADIUS(radius)   ((QString)("border-radius  : "  #radius  "px;\n"))

#define BORDER_RADIUS_TOP_LEFT(radius)      ((QString)("border-top-left-radius      : "  #radius  "px;\n"))
#define BORDER_RADIUS_TOP_RIGHT(radius)     ((QString)("border-top-right-radius     : "  #radius  "px;\n"))
#define BORDER_RADIUS_BOTTOM_RIGHT(radius)  ((QString)("border-bottom-right-radius  : "  #radius  "px;\n"))
#define BORDER_RADIUS_BOTTOM_LEFT(radius)   ((QString)("border-bottom-left-radius   : "  #radius  "px;\n"))

#define MAX_SIZE 16777215


struct QButton
{
	QPushButton * Button;
	QButton(QPushButton* bp) : Button(bp) {};

	template<typename cevent>
	QButton* click(cevent callback)
	{
		QObject::connect(Button, &QAbstractButton::clicked, callback);

		return this;
	}

	QButton * SetFont(BYTE size, bool bold)
	{
		QFont font;
		font.setPointSize(size);
		font.setBold(bold);
		Button->setFont(font);

		return this;
	}
};


struct QInputBox
{
	QLineEdit * Input;

	QInputBox(QLineEdit* input) : Input(input) {};

	template<typename cevent>
	QInputBox* WhenInput(cevent callback)
	{
		QObject::connect(Input, &QLineEdit::textChanged, callback);
		return this;
	}

	template<typename cevent>
	QInputBox* WhenEditFinish(cevent callback)
	{
		QObject::connect(Input, &QLineEdit::editingFinished, callback);

		return this;
	}

}; 


template<typename ST, typename  std::enable_if< std::is_same<ST,QDoubleSpinBox*>::value || std::is_same<ST, QSpinBox*>::value,bool>::type = true>
struct QNumberBox
{
	ST  SpinBox;

	QNumberBox(ST  spinbox) : SpinBox(spinbox) {};

	template<typename cevent>
	QNumberBox* WhenValueChanged(cevent callback)
	{
		if(std::is_same<ST, QDoubleSpinBox*>::value)
		     QObject::connect(dynamic_cast<QDoubleSpinBox*>(SpinBox), &QDoubleSpinBox::valueChanged, callback);

		else if (std::is_same<ST, QSpinBox*>::value)
			QObject::connect(dynamic_cast<QSpinBox*>(SpinBox), &QSpinBox::valueChanged, callback);


		return this;
	}
};


struct QtCheckBox
{
	QCheckBox* CheckBox;
	QtCheckBox(QCheckBox* checkbox) : CheckBox(checkbox) {};

	template<typename cevent>
	QtCheckBox* WhenStateChanged(cevent callback)
	{
		QObject::connect(CheckBox, &QCheckBox::stateChanged, callback);
		return this;
	}
};


struct QDropdownBox
{
	QComboBox * ComboBox;
	QDropdownBox(QComboBox * combobox) : ComboBox(combobox) {};

	template<typename cevent>
	QDropdownBox * WhenIndexChanged(cevent callback)
	{
		QObject::connect(ComboBox, &QComboBox::currentIndexChanged, callback);
		return this;
	}

	template<typename cevent>
	QDropdownBox * WhenTextChanged(cevent callback)
	{
		QObject::connect(ComboBox, &QComboBox::currentTextChanged, callback);
		return this;
	}

	QDropdownBox * AddItem(QString item)
	{
		ComboBox->addItem(item);
		return this;
	}

	QDropdownBox * AddItems(QStringList items)
	{
		ComboBox->addItems(items);
		return this;
	}
};

struct QBIcon : public QButton
{
	QBIcon(QPushButton* bp) : QButton(bp) {};

	QBIcon* SwitchIcon(QString Path)
	{
		Button->setIcon(QIcon(Path));
		return this;
	}
};

QT_BEGIN_NAMESPACE

class QtComponents : public QObject
{
public:

	static QFont CurrentFont(WORD Size = 12,bool IsBold = false,bool IsItalic=false,QString Font = "Bahnschrift SemiLight Condensed")
	{
		QFont f;
		f.setFamilies({Font});
		f.setPointSize(Size);
		f.setBold(IsBold);
		f.setItalic(IsItalic);

		return f;
	}


	static QLabel* AddText(QWidget* parent,QString text,WORD Size=12, bool Bold=false,bool IsItalic = false, QRect Geometry = { 0,0,0,0 }, QSize MinSize = { 0,0 }, QSize MaxSize = { MAX_SIZE , MAX_SIZE })
	{
		QLabel* t = new QLabel(parent);
		t->setText(text);
		t->setFont(QtComponents::CurrentFont(Size, Bold, IsItalic));
		t->setGeometry(Geometry);
		t->setMinimumSize(MinSize);
		t->setMaximumSize(MaxSize);
		t->show();

		return t;
	}

	static QWidget* AddWidget(QWidget* parent, QRect Geometry = { 0,0,0,0 }, QSize MinSize = { 0,0 }, QSize MaxSize = { MAX_SIZE , MAX_SIZE })
	{
		QWidget* widget = new QWidget(parent);
		widget->setGeometry(Geometry);
		widget->setMinimumSize(MinSize);
		widget->setMaximumSize(MaxSize);
		widget->show();
		return widget;
	}	

	static QAction* AddActionMenu(QMenu* parent, QString ActionText, bool Checkable = false, QString IconPath = "")
	{
		QAction* action = new QAction(parent);
		action->setText(ActionText);
		if (Checkable)
			action->setCheckable(Checkable);
		else  if (!Checkable)
			action->setIcon(QIcon(IconPath));

		parent->addAction(action);

		return action;
	}

	static QButton * AddButton(QWidget* parent,QString Text,WORD TSize=12, bool TBold=false, QRect Geometry = { 0,0,0,0 }, QSize MinSize = { 0,0 }, QSize MaxSize = { MAX_SIZE , MAX_SIZE })
	{
		QPushButton * button = new QPushButton(parent);
		button->setGeometry(Geometry);
		button->setMinimumSize(MinSize);
		button->setMaximumSize(MaxSize);
		button->setFont(QtComponents::CurrentFont());
		button->setText(Text);
		button->setFocusPolicy(Qt::StrongFocus);
		button->setFont(QtComponents::CurrentFont(TSize, TBold));
		button->show();
		return new QButton(button);

	}

	static QtCheckBox * AddCheckBox(QWidget* parent,QString Text, bool IsChecked = false, QRect Geometry = { 0,0,0,0 }, QSize MinSize = { 0,0 }, QSize MaxSize = { MAX_SIZE , MAX_SIZE })
	{
		QCheckBox * checkbox = new QCheckBox(parent);
		checkbox->setGeometry(Geometry);
		checkbox->setMinimumSize(MinSize);
		checkbox->setMaximumSize(MaxSize);
		checkbox->setChecked(IsChecked);
		checkbox->setText(Text);
		checkbox->setFont(QtComponents::CurrentFont(12,true,false));

		return new QtCheckBox(checkbox);

	}

	static QDropdownBox * AddComboBox(QWidget* parent, QRect Geometry = { 0,0,0,0 }, QSize MinSize = { 0,0 }, QSize MaxSize = { MAX_SIZE , MAX_SIZE })
	{
		QComboBox* ComboBox = new QComboBox(parent);
		ComboBox->setGeometry(Geometry);
		ComboBox->setMinimumSize(MinSize);
		ComboBox->setMaximumSize(MaxSize);
		ComboBox->setFont(QtComponents::CurrentFont());
		ComboBox->view()->window()->setWindowFlags(ComboBox->view()->window()->windowFlags() | Qt::NoDropShadowWindowHint);
		ComboBox->show();
		return new QDropdownBox(ComboBox);
	}

	static QInputBox * AddLineEdit(QWidget* parent,bool IsReadOnly = false, QRect Geometry = { 0,0,0,0 }, QSize MinSize = { 0,0 }, QSize MaxSize = { MAX_SIZE , MAX_SIZE })
	{
		QLineEdit * LineEdit = new QLineEdit(parent);
		LineEdit->setGeometry(Geometry);
		LineEdit->setMinimumSize(MinSize);
		LineEdit->setMaximumSize(MaxSize);
		LineEdit->setReadOnly(IsReadOnly);
		LineEdit->setFont(QtComponents::CurrentFont());
		LineEdit->show();
		return new QInputBox(LineEdit);
	}


	static QTabWidget * AddTabWidget(QWidget* parent, QRect Geometry = { 0,0,0,0 }, QSize MinSize = { 0,0 }, QSize MaxSize = { MAX_SIZE , MAX_SIZE })
	{
		QTabWidget* tabwidget = new QTabWidget(parent);
		tabwidget->setGeometry(Geometry);
		tabwidget->setMinimumSize(MinSize);
		tabwidget->setMaximumSize(MaxSize);
		tabwidget->setFont(QtComponents::CurrentFont(12, false,false,"Bahnschrift SemiBold"));
		tabwidget->show();

		return tabwidget;
	}

	static QNumberBox<QDoubleSpinBox*> * AddDoubleSpinBox(QWidget* parent, double MinValue = 0.000, double MaxValue = 99.000, double StartValue = 1.00, double StepValue = 1.000, QRect Geometry = { 0,0,0,0 }, QSize MinSize = { 0,0 }, QSize MaxSize = { MAX_SIZE , MAX_SIZE })
	{
		QDoubleSpinBox * SpinBox = new QDoubleSpinBox(parent);
		SpinBox->setGeometry(Geometry);
		SpinBox->setMinimumSize(MinSize);
		SpinBox->setMaximumSize(MaxSize);
		SpinBox->setMinimum(MinValue);
		SpinBox->setMaximum(MaxValue);
		SpinBox->setSingleStep(StepValue);
		SpinBox->setDecimals(3);
		SpinBox->setValue(StartValue);

		return new QNumberBox<QDoubleSpinBox*>(SpinBox);
		
	}

	static QBIcon* BpIcon(QWidget* parent, QString Path, QSize IconSize = { 32,32 }, QPoint Position = { 0,0 })
	{
		QPushButton* Icon = new QPushButton(parent);
		Icon->setObjectName(QString::fromUtf8("IconButton"));
		Icon->setGeometry(QRect(Position.x(), Position.y(), IconSize.width(), IconSize.height()));
		Icon->setMinimumSize(IconSize);
		Icon->setMaximumSize(IconSize);
		Icon->setIcon(QIcon(Path));
		Icon->setIconSize(IconSize - QSize(13, 13));
		Icon->setCursor(QCursor(Qt::PointingHandCursor));
		Icon->setFlat(false);
		Icon->show();
		return new QBIcon(Icon);
	}

	static QBIcon* BpIcon(QWidget* parent, QIcon Icon, QSize IconSize = { 32,32 }, QPoint Position = { 0,0 })
	{
		QPushButton* IIcon = new QPushButton(parent);
		IIcon->setObjectName(QString::fromUtf8("IconButton"));
		IIcon->setGeometry(QRect(Position.x(), Position.y(), IconSize.width(), IconSize.height()));
		IIcon->setMinimumSize(IconSize);
		IIcon->setMaximumSize(IconSize);
		IIcon->setIcon(Icon);
		IIcon->setIconSize(IconSize - QSize(13, 13));
		IIcon->setCursor(QCursor(Qt::PointingHandCursor));
		IIcon->setFlat(false);
		IIcon->show();
		return new QBIcon(IIcon);
	}

	static QLabel* Icon(QWidget* parent, QString Path, QSize IconSize = { 32,32 }, QPoint Position = { 0,0 })
	{
		QLabel * Icon = new QLabel(parent);
		Icon->setObjectName(QString::fromUtf8("IconButton"));
		Icon->setGeometry(QRect(Position.x(), Position.y(), IconSize.width(), IconSize.height()));
		Icon->setMinimumSize(IconSize);
		Icon->setMaximumSize(IconSize);
		Icon->setPixmap(QPixmap(Path));
		Icon->setScaledContents(true);
		Icon->show();
		return Icon;
	}

	static QLabel* Icon(QWidget* parent, QIcon Icon, QSize IconSize = { 32,32 }, QPoint Position = { 0,0 })
	{
		QLabel* IIcon = new QLabel(parent);
		IIcon->setObjectName(QString::fromUtf8("IconButton"));
		IIcon->setGeometry(QRect(Position.x(), Position.y(), IconSize.width(), IconSize.height()));
		IIcon->setMinimumSize(IconSize);
		IIcon->setMaximumSize(IconSize);
		IIcon->setPixmap(Icon.pixmap(Icon.availableSizes(QIcon::Normal)[0]));
		IIcon->setScaledContents(true);
		IIcon->show();
		return IIcon;
	}

};





#define INJECTOR_STYLESHEET R"(
#centralwidget
{
background-color: rgb(19, 19, 19);
border:1px solid white;
}

*>*
{
	color: rgb(75, 75, 75);
	selection-background-color: rgb(55, 55, 55);
}

#TitleBar
{
background-color: rgb(16, 16, 16);
}

#TitleBar QPushButton
{
border:none;
color:rgba(255,255,255,0.7);
padding-bottom:4px;
}
QPushButton
{
   border:none;
}


#BPMinimize
{
padding: 0 10px 4px 10px;
}

#BPClose
{
padding-top: 7px;
}

#BPClose::hover
{
   background-color: rgb(170, 0, 0);
}
#BPMinimize::hover
{
   background-color: rgba(255, 255, 255,0.04);
}

#injector QPushButton
{
color:rgb(55, 55, 55);
border-radius:5px;
}

#injector QPushButton::hover
{
  background-color: rgb(16, 16, 16);
}

#BPSaveOption,#BPRestartAdmin
{
    color:rgb(100,100,100);
}

#BPSaveOption::hover
{
   border-bottom : 1px solid rgb(90,90,90);
}

#BPRestartAdmin
{
    border-radius:4px;
     background-color: rgb(16, 16, 16);
}

#BPRestartAdmin::hover
{
     background-color: rgb(15, 15, 15);
}


QTabWidget  QWidget
{     
	background-color: transparent;
}

QTabWidget::pane {
    border:none;
	background-color: transparent;
}

QTabWidget::tab-bar {
    left: 2px; 
background-color: rgb(62, 187, 255);
}

QTabBar::tab {
     background-color: rgb(16, 16, 16);
     padding: 5px 15px 5px 15px;
     font-size:14px;
     color:rgba(255,255,255,0.7);
}

 QTabBar::tab:hover {
        background-color: rgb(14, 14, 14);
}

QTabBar::tab:selected {
  border-bottom: 2px solid rgba(255,255,255,0.15);
  background-color: rgb(14, 14, 14);
}


#BPInject
{
background-color: rgb(16, 16, 16);
padding: 0 10px 0px 0;
}

QComboBox
{
color: rgba(255, 255, 255,0.6);
border-radius:0px;
border-bottom:   2px solid rgb(30,30,30);
padding:0 0 0 3px;
}


QComboBox::drop-down {
    width: 20px;
}

QComboBox::down-arrow {
    width:15px;
	image: url(./Logos/down-arrow.png);
}

QComboBox QAbstractItemView {
    color: rgba(255, 255, 255,0.45);
	background-color: rgb(25,25,25);
    border:none;

}

QComboBox QAbstractItemView::item
{
   letter-spacing:1px;
   height:25px;
}

QLineEdit {
    border:none;
    border-bottom: 2px solid rgb(30,30,30);
    padding:0px 5px -5px 5px;
	color: rgba(255, 255, 255,0.6);
}

QCheckBox {
    letter-spacing: 1px;
    color:rgb(90,90,90);
}

QCheckBox::indicator {
    width: 12px;
    height: 12px;
   border: 1px solid rgb(70,70,70);
  border-radius:2px;
}

QCheckBox::indicator:unchecked {
   
}

QCheckBox::indicator:checked {
  background:  rgb(50,50,50);
}

#BPInject::hover
{
    border: 2px solid rgba(255,255,255,0.5);
}

QDoubleSpinBox {
	background-color: rgb(17, 17, 17);
    padding:2px 0 3px 5px;
    border:1px solid rgb(30,30,30);
}

QDoubleSpinBox::up-button {
	  image: url(./Logos/up.png);
      background-color: rgb(30, 30, 30);
      padding-top:2px;
      padding-bottom:2px; 
}

QDoubleSpinBox::down-button {
      image: url(./Logos/down.png);
      background-color: rgb(30, 30, 30);
      padding-top:2px;
      padding-bottom:2px;
}

)"


#define QMENU_STYESHEET R"(
QMenu
{
    background: rgb(35,35, 35);
    color:rgb(115,115,115);
}

QMenu::item {
    padding: 2px 25px 2px 20px;
    border: 1px solid transparent; 
}

QMenu::item:selected {
    background: rgb(30,30, 30);
}


QMenu::separator {
    height: 1px;
    background: rgb(45, 45, 45);
    margin-left: 10px;
    margin-right: 5px;
}

)"


QT_END_NAMESPACE
