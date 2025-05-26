# LeePass

KeePass provides a dll library that encrypts the data.  LeePass provides a front end for the user
that is more record oriented than spreadsheet oriented.

Most things are started using a toolbar command or combobox selection.  when a record is chosen it is
displayed in the window in edit boxes.  The dialog box containing the record displays the status
of the app on the bottom margin.  One specific status is read/write status of the app.  It begins
as "Read Only", i.e. any changes made to any record will not be saved.  The pencil icon on the
toobar toggles the Read Only/Save Record status.

This is an app in develpment progress.

## Getting Started

This version was created with Visual Studion 2022 (VS22).  It was compiled with the following properties:
  - Windows SDK Version: 10.0.xxxxx (latest)
  - Platfor Toolset: visual Studio 2022 (latest)
  - MFC: Use MFC in a Shared DLL
  - Character Set:  Use Unicode Character Set
  - Precompiled Header:  Not Using Precompiled Headers
  - Linker/Additional Dependencies:  Htmlhelp.lib

The HTML Help Workshop (HHW), Version 4.74.8702.0 (the last one produced) was used to prepare the help
file (WixApp.chm).  It is
copied into the Release directory.  I used Dreamweaver (DW) to do most of the content production of the
help files that the HTML Help Workshop produces (i.e. HHW is used to produce the pages/files needed
and DW is used to fill in the content).

Wix is used to create the &lt;AppName&gt;.msi file.  WixApp (one of my applications, see git) was used
to produce the product.wxs file.

Note:  In order to avoid hickups during the compilation/link process make sure the VS22 dependencies are
set so the projects are produced in the proper order.

## Prerequisites

Windows 7 or above.  visual Studio 2022 or above.  The WiX Toolset must be installed in Visual Studio.
The "HTML Help Workshop" (google it) must be installed on the PC.

### Installing

Execute the msi file to install the app.

## Updates

### Update 5/26/25

Trimed inputs of invisible characters (i.e. spaces/crlf).  Filled group combobox in entry form
for a new entry.  Revert to old group if user denies adding a new one when saving changes to a
record.

### Update 5/25/25

Checked and fixed
  - New Database -- Better initialization, defer saving file path until an entry is created
  - Open Database -- Better initialization, save file path immediately
  - Login -- Better initialization, Added logic to strings to handle zero TCchar pointers
  - Choose Group -- Better initialization, group names are kept in both cases but comparisons are
performed in one case.
  - New Entry -- Automatically suppy General as Group if not set by user.  Logic to reject entry
if there are not two fields present of Title, URL, UserName.

### Update 5/23/25

Embarked on a massive restructuring of the interface between LeePass and KeePass to protect
critical global variables.  Several files and functions were moved from LeePass to LeePassLib.
This is a work in progress as I am now examining all of the commands one by one.  Have looked at
New Database and Login pretty thouroughly.  Working my through New Entry.  The code is better now
but the command does not clear the dialog edit boxes.

Added some logic to be sure the labels do not get saved and if they do they get cleared eventually.

### Update 5/8/25

Some of the passwords did not fit in the edit box for passwords.  Also after loading a database
the opening width of the window cut off the exit button on the toolbar because the width of the
drop down lists of groups and entries was adjusted so that an entire entry was visible in the drop
down list.  I also discovered that the main dialob box reacted to an Enter key by exiting the
dialog box (this is normal behavior for a dialog).  So I've converted the Enter key in the main
dialog to a tab to the next field.

### Update 5/7/25

Added two radio buttons to the search dialog box.  The radio buttons specify either all groups or
just the current group are searched.  The search changes the current group in the event that all
groups are searched and the entry is found in a group that is not current.

### Update 5/6/25

While reviewing records in LastPass and LeePass (about 1/2 of them at this point) made several little
changes to "improve" the user interface.

### Update 5/3/25

Added the following menu items on the menu bar itself:
  - Open default browser with the url in the current record
  - Copy the user name to the clipboard
  - Copy the password for ~30 seconds to the clipboard.  The clipboard is erased if the app closes
or ~30 seconds has passed.

### 4/30/25

Added a password generator.  Added commands to start a URL in the default browser, copy the
username into the clip board and copy the password into the clip board.

### 4/28/25

Added Change Master Key command.  Added verification of master key before exporting data.

### 4/27/25

Added export to LandPass.  LandPass does not restore notes as they were when exported from
LandPass.  Shame on LandPass...

### 4/22/25 Update

Added expunge file after importing LandPass export.

### 4/21/25 Update

Added Find, Next, and move left and right commands.

### 4/16/25 Update

Added Access and Modification dates to entries.  Fixed getToday in date to get local date/time.

### 4/14/25 Update

Added commands to remove redundant groups, former last pass imports (&lt;tags&gt;), and all
backup group entries.  Fixed the icon to look a little like a safe.

### 4/11/25 Update

Add a command to remove duplicates (title, url and username are the same, the most recent created
saved).  There are some duplicates with the same title but different usernames or urls.

### 4/9/25 Update

This is a work in progress.  I managed to load my existing database into the app.  It will also
loaded an example LastPass export file into the app.  Next step is to load the full LastPass
export.  But for now this version will be uploaded.

## Authors

Robert R. Van Tuyl

Contact:  romeo romeo victor tango alpha tango sam whisky delta echo dot charlie oscar mike

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

