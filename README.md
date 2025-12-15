## 1. Objectifs du TP

Ce TP a pour objectif la mise en œuvre d’une commande en boucle ouverte d’une machine à courant continu (MCC) à l’aide d’un microcontrôleur STM32.
La commande repose sur la génération de PWM complémentaires avec temps mort, pilotées via une interface shell UART, permettant de contrôler la vitesse du moteur et d’observer son comportement expérimentalement.


## 2. Génération des PWM complémentaires (Commande MCC basique)

### 2.1 Configuration du timer

La génération des PWM est réalisée à l’aide d’un timer avancé configuré en :

![Config1](assets/Config1.jpg)

Ce mode permet une génération symétrique des fronts montants et descendants, adaptée au pilotage d’un pont de puissance.

### 2.2 Insertion du temps mort

Afin d’éviter tout court-circuit dans le pont de puissance, un temps mort est inséré entre les signaux complémentaires.

Horloge timer : 170 MHz  → période : 5.88 ns = 1 tic
Dead-time souhaité : 200 ns
Alors: `DTG = 200ns / 5.88ns ≈ 34`

Alors pour la configuration : Dead Time : 34

![Config2](assets/Config2.jpg)

## 3. Interface Shell UART

Une interface shell est mise en place via l’UART, permettant de commander le moteur en temps réel sans modifier le code.
La commande help permet d’afficher les commandes disponibles :

![Tera_help](assets/Tera_help.png)


![Oscillo1](assets/Oscillo1.jpg)

## 4. Commande de vitesse du moteur
### 4.1 Principe

La vitesse du moteur est commandée via la commande :  `speed XX `

où XX représente un pourcentage du rapport cyclique maximal.
La valeur est vérifiée afin de rester dans la plage autorisée (0–100).

### 4.2 Tests pour différentes consignes

Des essais ont été réalisés pour plusieurs valeurs de vitesse :

- speed 50
- speed 60
- speed 70

À chaque changement de consigne, la nouvelle valeur est correctement interprétée par le shell et appliquée au moteur.

4.3 Observation à l’oscilloscope

Les signaux PWM ont été observés à l’oscilloscope pour chaque consigne :

À 50 %, le fonctionnement est stable.

À 60 %, la variation du rapport cyclique est clairement visible.

À 70 %, une montée plus brusque est observée, illustrant l’augmentation de l’énergie fournie au moteur.

📸 Capture oscilloscope – PWM à 50 %

📸 Capture oscilloscope – PWM à 60 %
📸 Capture oscilloscope – PWM à 70 %










