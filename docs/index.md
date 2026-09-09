<!--- Github page main file --->

Pawmmit is a graphical Git client designed to help you understand and manage your source code history. 

Pawmmit is a fork of [Gittyup](https://github.com/Murmele/Gittyup), which is a continuation of the [GitAhead](https://github.com/gitahead/gitahead) client.

Stable versions for different packages are available for
- Flatpak for Linux [![Flathub](https://img.shields.io/badge/Flathub-gray?logo=Flathub&logoColor=23FBB04)](https://flathub.org/apps/details/com.github.Pawmmit.Pawmmit)
- [32 / 64 binary for Windows](https://github.com/Pawmmit/Pawmmit/releases/latest) or
- [macOS](https://github.com/Pawmmit/Pawmmit/releases/latest) or from [![homebrew](https://img.shields.io/badge/Homebrew-gray?logo=Homebrew&logoColor=%23FBB040)](https://formulae.brew.sh/cask/pawmmit)

The [latest development version](https://github.com/Pawmmit/Pawmmit/releases/tag/development) is available either as pre-built for
- flatpak for Linux,
- 32 / 64 binary for Windows,
- macOS

or, can be built from source by following the directions in the [Pawmmit Repository](https://github.com/Pawmmit/Pawmmit#how-to-build).

To see the changes of the current version please have a look at the <A href="#changelog">changelog</A> section

![Pawmmit](https://raw.githubusercontent.com/Pawmmit/Pawmmit/master/rsrc/screenshots/main_dark_orig.png)

How to Get Help
===============

Ask questions about building or using Pawmmit on
[Stack Overflow](http://stackoverflow.com/questions/tagged/pawmmit) by
including the `pawmmit` tag. Remember to search for existing questions
before creating a new one.

Report bugs in Pawmmit by opening an issue in the
[issue tracker](https://github.com/Pawmmit/Pawmmit/issues).
Remember to search for existing issues before creating a new one.

Multi language support
======================

Pawmmit supports the following languages:
- English (en)
- German (de)
- Spanisch (es)
- Japanese (ja)
- Portuguese (pt)
- Portuguese Brazil (pt_BR)
- Chinese (zh_CN)
- Russian (ru)

By default the system language is used. To switch to another language execute the application with the following command
```
LANG=<lang> <executable>
```


Features
========

### Single branch view to focus on your work
Select "Show Selected Branch" in the drop down menu above the commit list
![Single branch](https://raw.githubusercontent.com/Pawmmit/Pawmmit/master/rsrc/screenshots/main_show_selected_branch.png)

### Fullscreen
of the history or the change dialog by pressing Ctrl+M

### Tabs
to be able to switch fast between repositories

### Diff View
Staging and unstaging changes, viewing Blame
![Diff View](https://raw.githubusercontent.com/Pawmmit/Pawmmit/master/rsrc/screenshots/DiffView.png)

### Tree View
To visit the blame with its history for unchanged files

![Tree View](https://raw.githubusercontent.com/Pawmmit/Pawmmit/master/rsrc/screenshots/treeview.png)

### Blame View
See blame of the current version with an integrated timeline to see who changed which line

![Blame View](https://raw.githubusercontent.com/Pawmmit/Pawmmit/master/rsrc/screenshots/BlameView.png)

### Dynamic Line Wrapping
Courtesy of Scintilla.

![Line Wrapping](/rsrc/screenshots/line-wrap-demo-2.gif)

### Single line staging 
by eighter clicking on the checkboxes next to each line or by selecting the relevant code and pressing "S". For unstaging you can uncheck the checkboxes or press "U". To revert changes, select the text and press "R".

![Single line staging](https://raw.githubusercontent.com/Pawmmit/Pawmmit/master/rsrc/screenshots/double_treeview_single_line_staging.png)

### Amending commits
Editing properties of a commit
![Amend Dialog](https://raw.githubusercontent.com/Pawmmit/Pawmmit/master/rsrc/screenshots/AmendDialog.png)

### Solving rebase conflicts
Solving rebase conflicts and continuing after conflicts are solved

![Rebase Conflicts](https://raw.githubusercontent.com/Pawmmit/Pawmmit/master/rsrc/screenshots/RebaseConflicts.png)

### Starring commits
to find specific commits much faster
![Starring commits](https://raw.githubusercontent.com/Pawmmit/Pawmmit/master/rsrc/screenshots/starring_commits.png)

### Tag selection
Use an existing tag as template for your next tag. So you never have to look which is your latest tag

![Tag selection](https://raw.githubusercontent.com/Pawmmit/Pawmmit/master/rsrc/screenshots/tag_selection.png)

### Commit message template
Create you commit messages according a defined template. The first template is automatically applied to the commit message editor.

![Commit message template selection](https://raw.githubusercontent.com/Pawmmit/Pawmmit/master/rsrc/screenshots/CommitMessageTemplateSelection.png)

![Commit message template editor](https://raw.githubusercontent.com/Pawmmit/Pawmmit/master/rsrc/screenshots/CommitMessageTemplateEditor.png)

### And a lot more ...

Changelog
=========

{% include_relative changelog.md %}

