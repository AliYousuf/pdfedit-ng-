#ifndef __BASEGUI_H__
#define __BASEGUI_H__

#include "base.h"
#include <qmap.h>
#include <qobject.h>
#include <qstring.h>
#include <qvariant.h>

namespace gui {

class ColorTool;
class ConsoleWriterGui;
class PdfEditWindow;
class QSMenu;
class TreeItemAbstract;

//TODO: update documentation generator

/**
 Class that host scripts and contain static script functions<br>
 This class is also responsible for garbage collection of scripting
 objects and interaction of editor and scripts
 \brief Script hosting base class
*/
class BaseGUI : public Base {
 Q_OBJECT
public:
 BaseGUI(PdfEditWindow *parent);
 virtual ~BaseGUI();
 void runInitScript();
 QSCObject* treeitem();
 QSCObject* item();
 void addColorTool(ColorTool *tool);
 void treeItemDeleted(TreeItemAbstract* theItem);
public slots: //This will be all exported to scripting
 /*- Invokes "About" dialog, showing information about this program and its authors */
 void about();
 /*-
  Invokes dialog for adding additional objects to specified container (which must be <link linkend="type_Dict">Dictionary</link> or <link linkend="type_Array">Array</link>).
  After invoking dialog, this function returns immediately and the dialog is left for the user to use.
  If given object is not specified, curently selected object in property editor is used.
  If the object is not <link linkend="type_Dict">Dictionary</link> or <link linkend="type_Array">Array</link>, no dialog is shown.
 */
 void addObjectDialog(QSIProperty *container=NULL);
 void addObjectDialog(QObject *container);
 /*-
  Check (second parameter is true) or uncheck (false) item in toolbar and/or menu, given its name
  If you prefix name with slash ("/"), you will affect "class" of items
  - every item that belong to the specified class
  <note>
   Toolbuttons will automatically convert to Togglable toolbuttons this way
   and will start togling itself automatically on each succesive click
  </note>
 */
 void checkItem(const QString &name,bool check);
 /*- Closes all windows (and thus ends application) */
 void closeAll();
 /*-
  Closes file opened in this editor window.
  if askSave is true, user is asked to save the file if it is modified.
  if onlyAsk is true, file is not actually closed, only user is asked if he want to save work (if not specified, defaults to false).
  returns true in case of success, or false if user decide not to close current document and keep it open.
 */
 bool closeFile(bool askSave,bool onlyAsk=false);
 /*-
  Closes current editor window.
  If the file is not saved, user is asked to save changes to current file.
 */
 void closeWindow();
 /*- Creates new editor window with empty document in it. */
 void createNewWindow();
 /*-
  Enable (second parameter is true) or disable (false) item in toolbar and/or menu, given its name
  If you prefix name with slash ("/"), you will enable or disable "class" of items
  - every item that belong to the specified class
 */
 void enableItem(const QString &name,bool enable);
 /*-
  Return name of file loaded in editor window. If the file does not exist on disk
 (not loaded any file, or file was never saved, having no name), empty string is returned.
 */
 QString filename();
 /*- Invokes "open file" dialog and return selected filename, or NULL if dialog was cancelled */
 QString fileOpenDialog();
 /*-
  Invokes "save file" dialog and return selected filename, or NULL if dialog was cancelled.
  If file selected in dialog already exists, user is asked to confirm overwriting before returing its name.
 */
 QString fileSaveDialog(const QString &oldName=QString::null);
 /*-
  Get color from color picker with given name.
  Returns false if the color picker does not exist.
 */
 QVariant getColor(const QString &colorName);/*Variant=Color*/

 /*- Invokes program help. Optional parameter is topic - if invalid or not defined, help title page will be invoked */
 void help(const QString &topic=QString::null);
 /*-
  Check if part of the window is visible (returns true) or hidden (returns false)
  widgetName specifies which part:
  <informaltable frame="none">
   <tgroup cols="2"><tbody>
    <row><entry>commandline	</entry><entry>Command line</entry></row>
    <row><entry>rightside	</entry><entry>Right side tools (Tree Window and property editor)</entry></row>
    <row><entry>propertyeditor	</entry><entry>Property editor</entry></row>
    <row><entry>tree		</entry><entry>Tree window</entry></row>
    <row><entry>statusbar	</entry><entry>Statusbar</entry></row>
   </tbody></tgroup>
  </informaltable>
 */
 bool isVisible(const QString &widgetName);
 /*- Show simple messagebox with specified message and wait until user dismiss it */
 void message(const QString &msg);
 /*- Return true if the document was modified since it was opened or last saved, false otherwise. */
 bool modified();
 /*-
  Opens file with given name in this editor window.
  Opens without any questions, does not ask user to save changes to current file, etc ...
  Return true on success, false on failure to load file.
 */
 bool openFile(const QString &name);
 /*- Opens file with given name in new editor window */
 void openFileNew(const QString &name);
 /*- Invokes options dialog. Does not wait for dialog to be closed by user and return immediately. */
 void options();
 /*- Return currently shown page */
 QSPage* page();
 /*- Return page number of currently shown page */
 int pageNumber();
 /*-
  Invoke dialog to select color and return the color that user have picked,
  or false if user cancelled the dialog.
 */
 QVariant pickColor();/*Variant=Color*/
 /*-
  Create and return a <link linkend="type_Menu">Menu</link> object. The menuName parameter specifies
  name of item or list from configuration, that will be used to initially fill the menu with items.
  If this parameter is not specified or invalid, the menu will be initially empty.
  See documentation for <link linkend="type_Menu">Menu</link> object type for more info about
  using popup menus.
 */
 QSMenu* popupMenu(const QString &menuName=QString::null);
 /*- Show Yes/No question and wait for answer. Return true if user selected "yes", or false if user selected "no" */
 bool question(const QString &msg);
 /*-
  Show Yes/No/Cancel question and wait for answer.
  Return 1 if user selected "yes", 0 if user selected "no", -1 if user cancelled dialog (selecting Cancel)
 */
 int question_ync(const QString &msg);
 /*-
  Restore state of current editor window (size and position of window and elements inside it)
  State is restored from state saved in editor's configuration file.   
 */
 void restoreWindowState();
 /*-
  Save currently edited document.
  If current document have no name, user will be asked for name.
  If it is not possible to ask user or user presses cancel in file chooser,
  document is not saved.
  Return true if document was saved, false if it was not saved for any reason (file write error, user refused to give filename on new file ...)
 */
 bool save();
 /*-
  Save currently edited document (with currently active revision) under different name.
  Return true if document was saved, false if it failed to save for any reason
 */
 bool saveCopy(const QString &name);
 /*-
  Save currently edited document, while creating new revision in the process.
  Return true if document was saved, false if it was not saved for any reason.
 */
 bool saveRevision();
 /*-
  Save state of current editor window (size and position of window and elements inside it)
  State is saved to editor's configuration file.
 */
 void saveWindowState();
 /*- Change active revision in current PDF document */
 void setRevision(int revision);
 /*-
  Set part of the window to be either visible or invisible,
  widgetName specifies which part, see isVisible for list
  of possible names.
 */
 void setVisible(const QString &widgetName, bool visible);
 /*- Return root item of currently selected tree */
 QSTreeItem* treeRoot();
 /*- Return root item of main tree */
 QSTreeItem* treeRootMain();
 /*-
  Outputs given warning string to command window, followed by newline,
  and show this string in a messagebox to alert user.
 */
 void warn(const QString &str);

 //Tree selection related functions

 /*-
  Return first selected tree item.
  Set internal selected item pointer to first selected item
  For getting other selected tree items (in case more than one is selected, repeat calling nextSelectedItem() until NULL is returned (no more items)
  <example>
   <title>Getting through list of selected tree items</title>
   <programlisting>
    treeItem=firstSelectedItem();<br/>
    while (treeItem) {<br/>
    &nbsp;doSomething(treeItem);<br/>
    &nbsp;treeItem=nextSelectedItem();<br/>
    }
   </programlisting>
  </example>
  You can optionally specify different tree as parameter (by default, selected items from currently shown tree are taken)
  Specify "main" for main tree, "select" for selected operators or use number to specify number for Nth tree (indexed from zero).
  Omitting the parameter or specifying "current" will use currently shown tree
  If specified tree does not exist or its name is invalid, NULL is returned
 */
 QSTreeItem* firstSelectedItem(const QString &name=QString::null);
 /*-
  Return next selected tree item.
  Move internal selected item pointer to next selected item (or invalidate it if no more selected items is found)
 */
 QSTreeItem* nextSelectedItem();
 /*-
  Return object held in first selected tree item.
  Set internal selected item pointer to first selected item
  For getting other selected tree items (in case more than one is selected, repeat calling nextSelected() until NULL is returned (no more items)
  <example>
   <title>Getting through list of selected items</title>
   <programlisting>
    treeObject=firstSelected();<br/>
    while (treeObject) {<br/>
    &nbsp;doSomething(treeObject);<br/>
    &nbsp;treeObject=nextSelected();<br/>
    }
   </programlisting>
  </example>
  You can optionally specify different tree as parameter (by default, selected items from currently shown tree are taken)
  Specify "main" for main tree, "select" for selected operators or use number to specify number for Nth tree (indexed from zero).
  Omitting the parameter or specifying "current" will use currently shown tree
  If specified tree does not exist or its name is invalid, NULL is returned
 */
 QSCObject* firstSelected(const QString &name=QString::null);
 /*-
  Return object held in next selected tree item.
  Move internal selected item pointer to next selected item (or invalidate it if no more selected items is found)
 */
 QSCObject* nextSelected();

#ifndef DRAGDROP
private://This is workaround because of bug in MOC - it tries to include methods that are ifdef'ed out
#else
 // These are internal slots, should not available to scripting,
 // but it is not possible to do that. But at least they are uncallable
 void _dragDrop(TreeItemAbstract *source,TreeItemAbstract *target);
 void _dragDropOther(TreeItemAbstract *source,TreeItemAbstract *target);
#endif
private:
 QWidget* getWidgetByName(const QString &widgetName);
protected:
 //TODO: separate to BaseGuiCore
 virtual void addScriptingObjects();
 virtual void removeScriptingObjects();
 virtual void preRun(const QString &script,bool callback=false);
 virtual void postRun();
private:
 /** Console writer class writing to command window */
 ConsoleWriterGui* consoleWriter;
 /** Editor window in which this class exist */
 PdfEditWindow* w;
 /** Map with color picker tools */
 QMap<QString,ColorTool*> colorPickers;
};

} // namespace gui

#endif