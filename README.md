# 📦 Guide de Publication sur GitHub

## 🎯 Étapes Rapides

### 1️⃣ Préparation du Repository Local

```bash
cd ~/inf4097-projet/xv6-riscv

# Vérifier que Git est initialisé
git status

# Si pas de repo Git, initialiser
git init

# Vérifier les fichiers
git status
```

---

### 2️⃣ Nettoyer les Fichiers Compilés

**Important :** Nettoyer avant de commit pour éviter de pousser des binaires

```bash
# Nettoyer tous les fichiers compilés
make clean

# Vérifier qu'il ne reste pas de binaires
ls user/_*  # Ne devrait rien afficher
ls kernel/kernel  # Ne devrait pas exister
```

---

### 3️⃣ Créer les Fichiers Nécessaires

#### A. Copier le README.md

Copie le contenu du README depuis l'artifact et crée le fichier :

```bash
nano README.md
# Coller le contenu
# Ctrl+O pour sauvegarder
# Ctrl+X pour quitter
```

#### B. Créer le .gitignore

```bash
nano .gitignore
# Coller le contenu du .gitignore depuis l'artifact
# Ctrl+O, Ctrl+X
```

#### C. Vérifier la LICENSE

```bash
# Si LICENSE existe déjà, vérifier qu'elle est correcte
cat LICENSE

# Si elle n'existe pas ou est incorrecte, créer :
nano LICENSE
# Copier la MIT License (voir ci-dessous)
```

**MIT License Template :**
```
MIT License

Copyright (c) 2025 [Ton Nom]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

### 4️⃣ Créer le Repository sur GitHub

1. **Aller sur GitHub** : https://github.com
2. **Cliquer sur le "+" en haut à droite** → "New repository"
3. **Remplir les informations :**
   - **Repository name :** `xv6-enhanced-kernel`
   - **Description :** `Advanced OS features in xv6: system monitoring, thermal-aware scheduling, and lazy memory allocation`
   - **Visibility :** Public (ou Private si tu préfères)
   - **⚠️ NE PAS cocher "Initialize with README"** (tu en as déjà un)
   - **⚠️ NE PAS ajouter .gitignore** (tu en as déjà un)
   - **⚠️ NE PAS choisir de license** (tu en as déjà une)
4. **Cliquer sur "Create repository"**

---

### 5️⃣ Lier le Repository Local à GitHub

GitHub va te donner des commandes, utilise celles-ci :

```bash
# Dans ton dossier xv6-riscv

# Ajouter l'origin (remplace YOUR_USERNAME par ton username GitHub)
git remote add origin https://github.com/YOUR_USERNAME/xv6-enhanced-kernel.git

# Vérifier
git remote -v
```

---

### 6️⃣ Faire le Premier Commit

```bash
# Ajouter tous les fichiers (le .gitignore va filtrer automatiquement)
git add .

# Vérifier ce qui va être commité
git status

# Si des fichiers compilés apparaissent, les retirer :
git reset user/_*
git reset kernel/kernel
git reset fs.img

# Faire le commit initial
git commit -m "Initial commit: xv6 enhanced kernel with monitoring, heat-aware scheduler, and lazy allocation"

# Vérifier le commit
git log
```

---

### 7️⃣ Pousser sur GitHub

```bash
# Renommer la branche en main (standard GitHub)
git branch -M main

# Pousser vers GitHub
git push -u origin main
```

**Si demande d'authentification :**
- **Username :** Ton username GitHub
- **Password :** Utilise un **Personal Access Token** (pas ton mot de passe)

---

### 8️⃣ Créer un Personal Access Token (si nécessaire)

Si Git demande un mot de passe et refuse ton mot de passe normal :

1. **Aller sur GitHub** → Settings (icône profil en haut droite)
2. **Developer settings** (tout en bas à gauche)
3. **Personal access tokens** → **Tokens (classic)**
4. **Generate new token** → **Generate new token (classic)**
5. **Cocher :** `repo` (accès complet aux repos)
6. **Generate token**
7. **Copier le token** (tu ne le reverras plus !)
8. **Utiliser ce token comme mot de passe** dans Git

---

### 9️⃣ Vérifier sur GitHub

1. **Aller sur** : `https://github.com/YOUR_USERNAME/xv6-enhanced-kernel`
2. **Vérifier que :**
   - Le README.md s'affiche correctement
   - Les fichiers source sont là
   - Pas de binaires (user/_*, kernel/kernel, fs.img)
   - La structure est correcte

---

## 🎨 Personnaliser le README

Avant de pousser, personnalise ces sections dans README.md :

```markdown
# Ligne 155 - Remplace par ton nom
**Author:** [Your Name]  

# Ligne 159 - Remplace par l'URL de ton repo
**Project Link:** [https://github.com/YOUR_USERNAME/xv6-enhanced-kernel]
```

---

## 🔄 Commits Futurs

Pour les modifications futures :

```bash
# Faire des changements dans le code
nano kernel/sysproc.c

# Compiler et tester
make qemu

# Nettoyer avant commit
make clean

# Ajouter les changements
git add kernel/sysproc.c

# Commiter avec message descriptif
git commit -m "Fix: correct cpu_percent calculation in getactivity()"

# Pousser
git push
```

---

## 📋 Bonnes Pratiques de Commit

### Messages de Commit Clairs

```bash
# Format recommandé : <type>: <description>

git commit -m "feat: add lazy allocation support"
git commit -m "fix: resolve page fault in lazy_alloc()"
git commit -m "docs: update README with installation steps"
git commit -m "refactor: optimize scheduler heat calculation"
git commit -m "test: add benchmarks for heat-aware scheduler"
```

**Types courants :**
- `feat` : Nouvelle fonctionnalité
- `fix` : Correction de bug
- `docs` : Documentation
- `refactor` : Refactoring sans changement fonctionnel
- `test` : Ajout/modification de tests
- `perf` : Amélioration de performance
- `chore` : Tâches diverses (build, CI, etc.)

---

## 🌿 Workflow Branches (Optionnel)

Pour un projet plus organisé :

```bash
# Créer une branche pour une nouvelle feature
git checkout -b feature/improve-scheduler

# Travailler dessus
# ...

# Commit
git add .
git commit -m "feat: implement adaptive heat threshold"

# Pousser la branche
git push -u origin feature/improve-scheduler

# Sur GitHub : Créer une Pull Request
# Merger dans main une fois approuvé
```

---

## 🆘 Commandes de Dépannage

### Annuler le dernier commit (avant push)
```bash
git reset --soft HEAD~1
```

### Voir les différences
```bash
git diff
git diff --staged  # Pour les fichiers déjà ajoutés
```

### Retirer un fichier du staging
```bash
git reset HEAD fichier.c
```

### Voir l'historique
```bash
git log --oneline --graph
```

### Ignorer un fichier déjà tracké
```bash
git rm --cached fichier.o
echo "fichier.o" >> .gitignore
git commit -m "chore: remove compiled file from tracking"
```

---

## ✅ Checklist Avant Push

- [ ] `make clean` exécuté
- [ ] README.md personnalisé (nom, liens)
- [ ] .gitignore présent
- [ ] LICENSE présente
- [ ] Pas de fichiers compilés dans `git status`
- [ ] Code compile et fonctionne
- [ ] Tests passent
- [ ] Message de commit descriptif

---

## 🎓 Ajouter des Badges (Optionnel)

Ajoute des badges sympas au README :

```markdown
![Build Status](https://img.shields.io/badge/build-passing-brightgreen)
![Tests](https://img.shields.io/badge/tests-passing-brightgreen)
![Coverage](https://img.shields.io/badge/coverage-85%25-yellowgreen)
```

---

## 📸 Ajouter des Screenshots (Optionnel)

Crée un dossier pour les captures :

```bash
mkdir -p docs/screenshots

# Ajoute tes captures d'écran
cp ~/Downloads/activitymon-output.png docs/screenshots/

# Dans le README.md, ajoute :
# ![Activity Monitor](docs/screenshots/activitymon-output.png)
```

---

## 🚀 C'est Parti !

Tu es maintenant prêt à pousser ton projet sur GitHub. Bonne chance ! 🎉

Si tu as des problèmes, vérifie :
1. Connexion Internet
2. Token GitHub valide
3. Nom du repo correct
4. Pas de conflits de fichiers