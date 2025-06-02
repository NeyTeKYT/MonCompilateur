# 🚀 AMS - Création d’un Compilateur C++ pour un Mini-Pascal 🧠

Bienvenue sur le dépôt de mon projet de création d'un compilateur C++ pour un mini-Pascal.

Ce projet a été réalisé dans le cadre de l'**Activité de Mise en Situation (AMS)** du second semestre de ma **deuxième année de licence en informatique** à l'université d’*Avignon*.

🔍 Ce travail m’a permis de comprendre en profondeur la lecture et l'analyse de programmes écrits dans des **langages compilés** comme C++.  

🧩 J’ai conçu et implémenté **plusieurs instructions inspirées du langage Pascal**, avec une génération automatique du **code assembleur**, le tout en C++.

🛠️ Tous les programmes de test `.p` sont pris en charge grâce à un `Makefile` qui compile automatiquement le programme, détecte les erreurs de syntaxe ou de structure, et aide l'utilisateur à les corriger.

---

## 🧾 Instructions et Fonctionnalitées Implémentées

Le compilateur prend en charge les instructions suivantes :

- 🔁 `WHILE ... DO ...`  
- 🔁 `FOR ... TO ... DO ...`  
- ❓ `IF ... THEN ... [ELSE]`  
- 🧮 `CASE ... OF ... END`  
- 🔡 `DISPLAY`  
- 🔣 `VAR`  
- 🧱 `BEGIN ... END`

✔️ Toutes ces instructions sont **analysées, validées et traduites en assembleur**, prêtes à être exécutées ou interprétées.

💡 La génération du code assembleur pour ces différentes instructions est fortement inspirée des page 33-35 du cours.

📚 Les fonctions ont été documentées dans le style de la **JavaDoc**, pour faciliter la relecture et la maintenance du code

📐 Certaines fonctions ont été crées pour améliorer la lisibilité du code.
➡️ Par exemple, le bout de code `current = (TOKEN) lexer->yylex();`, qui m'a semblé pas très intuitif, a été encapsulé dans `readCurrent()`.

---

## 🧪 Test du projet

Voici les commandes principales disponibles via le `Makefile` :

- `make clean` : supprime les fichiers générés précédemment pour repartir de zéro. 🧹
- `make` : compile tous les fichiers `.p`, vérifie la syntaxe des programmes et génère le code assembleur correspondant. 🔧

