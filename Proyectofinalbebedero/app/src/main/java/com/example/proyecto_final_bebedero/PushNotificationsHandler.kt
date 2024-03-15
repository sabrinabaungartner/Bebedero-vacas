package com.example.proyecto_final_bebedero

import android.app.Activity
import android.app.NotificationChannel
import android.app.NotificationManager
import android.content.Context
import android.content.pm.PackageManager
import androidx.core.app.ActivityCompat
import androidx.core.app.NotificationCompat
import androidx.core.app.NotificationManagerCompat

class PushNotificationsHandler(private val context: Context): PushNotificationsInterface {
    companion object {
        const val MY_CHANNEL_ID = "my_channel_bebedero"
        const val PERMISSION_REQUEST_NOTIFICATION = 100
    }

    val GROUP_KEY_BEBEDERO = "group_app_bebedero"

    override fun createNotificationChannel(title: String, channelDescription: String) {
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.O) {
            val importance = NotificationManager.IMPORTANCE_DEFAULT
            val channel = NotificationChannel(MY_CHANNEL_ID, title, importance).apply {
                description = channelDescription
            }
            val notificationManager: NotificationManager =
                context.getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
            notificationManager.createNotificationChannel(channel)
        }
    }

    override fun showNotification(title: String, textToShow: String) {
        val builder = NotificationCompat.Builder(context, MY_CHANNEL_ID)
            .setSmallIcon(android.R.drawable.ic_dialog_info)
            .setContentTitle(title)
            .setContentText(textToShow)
            .setPriority(NotificationCompat.PRIORITY_DEFAULT)
            .setGroup(GROUP_KEY_BEBEDERO)

        checkAndRequirePermissionsToNotify(builder)
    }

    private fun checkAndRequirePermissionsToNotify(builder: NotificationCompat.Builder) {
        if (ActivityCompat.checkSelfPermission(
                context,
                android.Manifest.permission.POST_NOTIFICATIONS
            ) != PackageManager.PERMISSION_GRANTED
        ) {
            ActivityCompat.requestPermissions(
                context as Activity,
                arrayOf(android.Manifest.permission.POST_NOTIFICATIONS),
                PERMISSION_REQUEST_NOTIFICATION
            )
        } else {
            with(NotificationManagerCompat.from(context)) {
                notify(1, builder.build())
            }
        }
    }
}