<!--
*** Thanks for checking out the controle-souris-webcam. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Thanks again! Now go create something AMAZING! :D
-->

<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]
[![LinkedIn][linkedin-shield]][linkedin-url]

<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/camille/controle-souris-webcam">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

  <h3 align="center">Contrôle souris webcam</h3>

  <p align="center">
    Un programme en c++ pour contrôler votre souris avec les mouvements de votre tête !
    <br />
    <a href="https://github.com/camillevingere/controle-souris-webcam"><strong>Explorer la doc »</strong></a>
    <br />
    <br />
    <a href="https://github.com/camillevingere/controle-souris-webcam">Voir la démo</a>
    ·
    <a href="https://github.com/camillevingere/controle-souris-webcam/issues">Reporter un bug</a>
    ·
    <a href="https://github.com/camillevingere/controle-souris-webcam/issues">Reporter une fonctionnalité</a>
  </p>
</p>

<!-- TABLE OF CONTENTS -->
<details open="open">
  <summary>Table des matières</summary>
  <ol>
    <li>
      <a href="#le-projet">Le projet</a>
      <ul>
        <li><a href="#réalisé-avec">Réalisé avec</a></li>
      </ul>
    </li>
    <li>
      <a href="#pour-commencer">Pour commencer</a>
      <ul>
        <li><a href="#prérequis">Prérequis</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
        <li>
      <a href="#recherche-de-solution">Recherche de solution</a>
      <ul>
        <li><a href="#manette">Manette</a></li>
        <li><a href="#limites">Limites</a></li>
        <li><a href="#pave-tactile">Pavé tactile</a></li>
      </ul>
    </li>
            <li>
      <a href="#calcul-de-l-orientation-de-la-tête">Calcul de l'orientation de la tête</a>
      <ul>
        <li><a href="#technique-n-1">Technique n°1</a></li>
        <li><a href="#technique-n-2">Technique n°2</a></li>
        <li><a href="#technique-n-2">Technique n°3</a></li>
      </ul>
    </li>
    <li><a href="#contact">Contact</a></li>
  </ol>
</details>

<!-- ABOUT THE PROJECT -->

## Le projet

Polytech’Lille, SE4, IHM

Le but de ce TP est de mettre en place une petite application permettant de
controler de curseur avec l’orientation de la tête.

### Build with

Ce projet a été réalisé avec les technologies suivantes

- [C++](https://fr.wikipedia.org/wiki/C%2B%2B)
- [OpenCV](https://opencv.org/)
- [Xlib](https://fr.wikipedia.org/wiki/Xlib)

<!-- GETTING STARTED -->

## Pour commencer

Il est nécéssaire d'installer la bibliothèque d'OpenCV version 4.2.

### Pré-requis

Vous pouvez les installer comme ceci.

- OpenCV
  ```sh
  apt install libopencv-dev
  ```
- Pour connaître la version
  ```sh
  pkg-config --modversion opencv
  ```

### Installation

1. Cloner le dépôt
   ```sh
   git clone https://github.com/camillevingere/controle-souris-webcam.git
   ```
2. Choisir la technique utilisée. Exemple :
   ```sh
   cd Technique-2
   ```
3. Compiler le .cpp (il n'est pas nécessaire de modifier le fichier compile.sh)
   ```sh
   ./compile.sh
   ```
4. Lancer le projet
   ```sh
   ./output
   ```

## Recherche de solution

### Manette

Une approche similaire à celui d'une manette pourrait être intéressant. En effet la manette propose un joystick qui, grâce à l'orientation de celui nous donne une direction (haut, bas, gauche, droite).

### Limites

Cette méthode ne permet pas de contrôler la souris dans le noir (chose qu'on peut faire avec une "vraie" souris). Elle fonctionnera sûrement moins bien avec les personnes qui portent des lunettes (à cause des reflets) et encore moins qui portent des lunettes de soleil.

Elle limite aussi la possiblité d'utilisation prolongée. Elle fatigue l'utilisateur qui l'utilise.

Dans l'hypothèse que la personne qui utilise cette méthode ne puisse pas utiliser la souris "lambda" on peut également supposer qu'elle ne puisse pas utiliser le clavier. Est-ce utile pour un utilisateur d'avoir un ordinateur sans clavier ? Est-ce possible de trouver une alternative ?

Pour les chiffres on pourrait éventuellement découper l'écran en 9 carrés et positionner la tête dans les carrés pour représenter un chiffre.

### Pave tactile

Il est possible d'implémenter quelque chose de similaire au pavé tactile. Cependant on utilisera plus l'orientation de la tête mais plutôt sa position dans un espace 2D.

<!-- USAGE EXAMPLES -->

## Calcul de l'orientation de la tête

Pour le calcul de l'orientation de la tête j'ai utilisé 3 différentes techniques pour pouvoir les comparer entre elles.

### Technique n°1

Cette technique est très simple et basique. On récupère la position de la tête et on rajoute une ellipse (afin de montrer à l'utilisateur que la tête est bien détectée). On réalise la même chose pour les yeux.

Une fois que ceci est fait on récupère la position de la tête sur l'axe x et y et on déplace la souris.

La limite de cette technique se trouve dans la calibration du mouvement de la souris. En effet, il faut un déplacement de tête conséquent pour déplacer la souris d'un côté de l'écran à l'autre (c'est même impossible). De plus la souris n'est pas initialisée au centre de l'écran donc la souris se retrouve presque dans un des coins de l'écran.

### Technique n°2

Cette technique est plus élaborée et représentera mon rendu final.

1. Échantillonage

Premièrement on récupère la position du visage sur 20 frames afin de déterminer un centre moyen de la tête sur l'écran. On retient ce centre moyen dans une variable globale qui ne changera plus. Une amélioration possible serait de pouvoir proposer à l'utilisateur un nouvel échantillonage du centre si celui-ci le souhaite.

2. Calcul de l'orientation gauche/droite

On récupère une nouvelle frame. On récupère le centre du retangle représentant la position de la tête de l'utilisateur sur l'axe x. On le compare à notre centre moyen calculé précédemment.

Si le centre moyen moins le centre actuel est supérieur à 0 alors la souris doit aller à gauche, sinon elle doit aller à droite.

3. Calcul de l'orientation haut/bas

Même chose sauf qu'on récupère pas le centre moyen et le centre du rectangle mais le centre du cercle de l'oeil.

Cette partie ne fonctionne pas très bien. Il aurais fallut que je fasse également un centre moyen pour les yeux si je voulais utiliser la même technique.

4. Calibration

Cette fois on utilise une fonction calibration qui va adapter le déplacement de la souris proportionnellement au mouvement de la tête. On utilise juste un rapport de déplacement calculé avec la différence entre l'ancienne position et la nouvelle.

### Technique n°3

Cette technique est une technique récupérée directement sur internet. Cette technique utilise la position de l'iris des yeux pour effectuer tous les déplacements.

Critique : Cette technique peut effectivement faire se déplacer la souris mais le calibrage doit être adapté. En effet, l'algorithme le plus intéressant est celui qui combinera précision et efficacité. Donc on privilégiera un algorithme qui ne demande pas trop d'énergie à l'utilisateur.

<!-- CONTACT -->

## Contact

Camille Vingere - camille.vingere@gmail.com

Project Link: [https://github.com/camillevingere/controle-souris-webcam](https://github.com/camillevingere/controle-souris-webcam)

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->

[contributors-shield]: https://img.shields.io/github/contributors/camillevingere/controle-souris-webcam.svg?style=for-the-badge
[contributors-url]: https://github.com/camillevingere/controle-souris-webcam/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/camillevingere/controle-souris-webcam.svg?style=for-the-badge
[forks-url]: https://github.com/camillevingere/controle-souris-webcam/network/members
[stars-shield]: https://img.shields.io/github/stars/camillevingere/controle-souris-webcam.svg?style=for-the-badge
[stars-url]: https://github.com/camillevingere/controle-souris-webcam/stargazers
[issues-shield]: https://img.shields.io/github/issues/camillevingere/controle-souris-webcam.svg?style=for-the-badge
[issues-url]: https://github.com/camillevingere/controle-souris-webcam/issues
[license-shield]: https://img.shields.io/github/license/camillevingere/controle-souris-webcam.svg?style=for-the-badge
[license-url]: https://github.com/camillevingere/controle-souris-webcam/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://www.linkedin.com/in/camille-vingere/
[product-screenshot]: images/screenshot.png
