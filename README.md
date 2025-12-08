## 1. Objectifs du TP

L’objectif de ce projet est de prendre en main un système d’actionnement complet basé sur une carte Nucleo-STM32G474RE et un hacheur 4 transistors.  
Les travaux à réaliser sont les suivants :

1. Générer la commande des 4 transistors du hacheur en PWM complémentaire décalée.
2. Assurer l’acquisition des différents capteurs du système.
3. Implémenter un asservissement en temps réel.

## 2. Génération des PWM complémentaires (Commande MCC basique)

L’objectif est d’obtenir quatre signaux PWM :
- deux principaux (U, V)
- deux complémentaires (U’, V’)
- tous décalés avec un dead-time pour éviter le shoot-through

### 2.1. Paramétrage de TIM1

![Config1](assets/Config1.jpg)
![Config2](assets/Config2.jpg)

### 2.2 Calcul du Dead-Time

Horloge timer : 170 MHz  
→ période : 5.88 ns

Dead-time souhaité : 200 ns
Alors: `DTG = 200ns / 5.88ns ≈ 34`

### 2.3 Résultat oscilloscope

![Oscillo1](assets/Oscillo1.jpg)


## 3. Contrôle moteur via shell UART

Le mini-shell permet au user de contrôler le moteur depuis un terminal série.

### Commandes disponibles

| Commande | Description |
|----------|-------------|
| `start` | Active la génération PWM |
| `stop` | Désactive la PWM |
| `speed <val>` | Change la consigne de rapport cyclique |
| `help` | Liste les commandes disponibles |







