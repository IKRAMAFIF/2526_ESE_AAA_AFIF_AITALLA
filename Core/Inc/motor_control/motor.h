/*
 * motor.h
 *
 *  Created on: Nov 11, 2025
 *      Author: nicolas
 */

#ifndef INC_MOTOR_CONTROL_MOTOR_H_
#define INC_MOTOR_CONTROL_MOTOR_H_


#include "main.h"

// Fonctions de contrôle du Moteur DC
// Commande complémentaire décalée (Bipolaire) sur TIM1 CH1(U) et CH2(V)

/**
 * @brief Démarre les canaux PWM et PWM_N (Complémentaires)
 */
void Motor_Start(void);

/**
 * @brief Arrête les canaux PWM (Moteur en roue libre ou arrêt)
 */
void Motor_Stop(void);

/**
 * @brief Définit la vitesse et le sens du moteur selon le paramètre Alpha.
 * @param alpha : Rapport cyclique de commande (float entre 0.0 et 1.0)
 * - alpha = 0.5 : Arrêt (Vitesse nulle, 0V moyen)
 * - alpha > 0.5 : Sens Avant (jusqu'à 1.0 pour Vitesse Max)
 * - alpha < 0.5 : Sens Arrière (jusqu'à 0.0 pour Vitesse Max)
 */
void Motor_SetSpeed(float alpha);


#endif /* INC_MOTOR_CONTROL_MOTOR_H_ */
