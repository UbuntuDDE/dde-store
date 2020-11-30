# Translation Guide

To start translating, create a fork of this repository.

## .ts file
In the `/translations` folder, you'll see several files with the extension `.ts`. Make a copy of `dde-store.ts` or `dde-store_en.ts` and rename it to `dde-store_yourlanguagecode.ts` (Spanish = `dde-store_es.ts`, German = `dde-store_de.ts`, etc). Open the file and change the `language="en"` in the first line to match the same language code.
```xml
<?xml version="1.0" ?><!DOCTYPE TS><TS language="en" version="2.1">
```

Every piece of text that needs to be translated comes in a format like this:
```xml
<message>
  <location filename="../main.cpp" line="17"/>
  <source>Created by %1</source>
  <translation>Created by %1</translation>
</message>
```
Anything between the `<source></source>` tags is the original text. Don't change it.

Anything between the `<translation></translation>` tags is what needs to be translated.

See how this example says `Created by %1`? %1 is text that will the app will replace. In this case, %1 will be changed to my name. You can move around %1 to wherever is gramatically correct, just don't remove it.

Some things may be repeated, like the categories or "OK". You can just copy the same translation as before.

Make sure to not translate anything else like the `<context></context>` tags!

## README.md
Create a copy of `README.md` and rename it to `README.languagecode.md`. Delete the language selector at the top (the flags and languages) and the rest is pretty simple. Just translate it! Just don't change `DDE Store`. The name needs to stay. Once you're done, move it to `translations/README/`, and add the flag, name, and link to the main README's language selector formatted the same way as the rest.

## .desktop file
Edit `dde-store.desktop` and 2 new lines: `Name[languagecode]=` and the translation of "App Store", and `Comment[languagecode]=` and the same translation of "An app store for DDE built with DTK" you put in the README. Make sure the lines are inserted alphabetically by language code.


If you're confused about anything, take a look at another translation to see what they changed, or just ask me. Once you're done with both files, just submit a pull request. I'll make sure everything looks fine and merge it. Thank you for taking interest in contributing!
