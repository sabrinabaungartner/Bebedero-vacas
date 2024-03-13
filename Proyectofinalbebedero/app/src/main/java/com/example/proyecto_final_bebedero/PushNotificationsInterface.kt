package com.example.proyecto_final_bebedero

interface PushNotificationsInterface {
    fun createNotificationChannel(title: String, channelDescription: String)
    fun showNotification(title: String, textToShow: String)
}