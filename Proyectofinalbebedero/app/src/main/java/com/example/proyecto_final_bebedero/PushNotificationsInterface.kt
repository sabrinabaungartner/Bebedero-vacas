package com.example.proyecto_final_bebedero

interface PushNotificationsInterface {
    fun createNotificationChannel(title: String, channelDescription: String)
    //fun showNotification(title: String, shortTextToShow: String, largeTextToShow: String, groupKey: String)
    fun showNotificationTemperature(title: String, shortTextToShow: String, largeTextToShow: String)
    fun showNotificationQuality(title: String, shortTextToShow: String, largeTextToShow: String)
    fun showSummaryNotification()
}