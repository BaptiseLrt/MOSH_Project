# MOSH_Project
5ISS_MOSH_LESSONS

# Mini-Project

# Piste bleue

Objectifs:

1) Réaliser un prototype rapide sur breadboard qui permet d'envoyer les données du capteur de gaz sur The Thing Networks (https://www.thethingsnetwork.org/)

2) Réalisation du shield grâce à KiCAD

(fait en cours)

3) Gestion d’une interruption sur le capteur de gaz signalant la présence d’un danger

On souhaite réaliser une interruption analogique sur le pin 2 lorsqu'un danger est detecté. Dans le cadre du tp, nous fixons donc un seuil de dangerosité à 0.7V. 
Sachant que l'entrée analogique du pin 2 considère l'état haut à 0.7 Vcc et l'état bas 0.3 Vcc, nous devons implémenter un comparateur d'intensité qui délivre au moins 0.7Vcc lorsque le capteur de gaz fournit 0.7 V.

Nous avons choisi un montage comparateur non inverseur avec un AOP, 2 résistances et une led qui s'allume lorque l'interruption est levée:

(insérer schéma et calculs voir ipad)


# Piste rouge

On doit réduire la conso d'énergie au maximum 

Avec le montage précédent nous consommons

0.3A au setup et 0.27 A en fonctionnement

sleep mode
power off certains trucs

Après avoir activé le sleep mode lorsqu'aucun danger n'est detecté et en ayant désactivé tous les composants dont nous ne nous servons pas (spi, i2c, etc) on descend à 0.04A (sans le capteur de temperature) et 0.24A avec le capteur.

on peut essayer de ralentir clock arduino
