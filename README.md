# Qt Auto Translator

![Logo](Resources/Logo.png)

Qt Auto Translator is an semi-automated tool to translate a TS file of a Qt Project.

It will extract the entries from a .ts file and generate a simple text file, which you can upload to an online translator (i.e. Google Translate). You can save its output and feed it back to Qt Auto Translator, and it will automatically fill the ts with the corresponding translations.

You also can provide already translated entries which will override the online translations. This is important when you have a set of phrases which online translation tools cannot translate.

## How to use it?


* In order to use this tool, your Qt Application should be localized.  
    You must use `tr()` to enclose statis texts.  
    i.e. `this->setTitle(tr("About"));`

* Then use the lupdate command to update the ts file.  
    You may also need to add a translation file, if you already don’t have one.  
    visit here for more info : http://doc.qt.io/qt-4.8/linguist-manager.html  
* After opening Qt Auto Translator, locate the `.ts` file that you want to translate.
* Then click on "Generate Online Translatable File"  
    - It will give you a simple text file with all the entries inside the .ts file.  
    - Save the file. (i.e. `Online.txt`)
* Translate the file using an Online Translation Tool.  
    - Go to the online translator (i.e. Google Translate)  
    - There is an option to upload a `.txt` file. Upload the `.txt` which is generated from the above step.  
    - Save the output in another `.txt` file. (i.e. `Translated.txt`)
* Apply the translations  
    - Open the Qt Auto Translator and you can see there are two text fields to locate the `generated.txt` file and the `translated.txt` file. Locate them using the corresponding browse buttons.  
    - If you already have a set of pre-translated phrases (which are already in the `.ts` file), locate that file using the "Manually Translated File" option. This file must contain value pairs as phrase & translated text.  
    - The "Entry Separator String" in the Settings section will be used to extract these entries. Hence, make sure that the entries in your file is separated using that string.
* Generate a Manual File for further usage  
    If you need to translate any phrases manually, generate a manual file and translate them. Then you can use it to update the `.ts` file.
