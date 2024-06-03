package com.example.proyecto_final_bebedero

import android.annotation.SuppressLint
import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.view.MenuItem
import android.widget.Button
import android.widget.ImageButton
import android.widget.ProgressBar
import android.widget.SeekBar
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.cardview.widget.CardView
import androidx.drawerlayout.widget.DrawerLayout
import com.google.android.material.navigation.NavigationView

class MainActivity : AppCompatActivity() {
    private val firebaseDatabaseInterface: FirebaseDatabaseInterface = FirebaseDatabaseHandler()
    private lateinit var notificationHandler: PushNotificationsInterface

    private var maxTemperature: Double = 0.0
    private var minWaterLevel: Int = 0
    private var waterQuality: Int = 0
    private var maxDaysWithoutFill: Int = 0 //5
    private var medDaysWithoutFill: Int = 0 //2
    private var userNotifiedAboutWaterTemperature: Boolean = false
    private var userNotifiedAboutWaterQuality: Boolean = false
    private var userNotifiedAboutWaterLevel: Boolean = false
    private val qualityLectureError: Int = 99

    // Vars for water temperature
    private lateinit var waterTemperatureView: CardView
    private lateinit var progressBarWaterTemperature: ProgressBar
    private lateinit var progressBarWaterTemperatureText: TextView
    private lateinit var textViewWaterTemperature: TextView

    // Vars for water level
    private lateinit var waterLevelView: CardView
    private lateinit var textViewWaterLevel: TextView

    // Vars for water quality
    private lateinit var waterQualityView: CardView
    private lateinit var textViewWaterQuality: TextView

    // SeekBars
    private lateinit var seekBarWaterTemperature: SeekBar
    private lateinit var seekBarTextViewWaterTemperature: TextView
    private lateinit var seekBarWaterQuality: SeekBar
    private lateinit var seekBarTextViewWaterQuality: TextView

    private lateinit var drawerLayout: DrawerLayout
    private lateinit var navView: NavigationView
    private lateinit var menuButton: ImageButton
    private lateinit var fillButton: Button

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setupMainView()

        navigationDrawerSetup()

        initWaterTemperatureViews()
        initWaterLevelViews()
        initWaterQualityViews()

        notificationHandler = PushNotificationsHandler(this) // Initialize the notification handler
        notificationHandler.createNotificationChannel("Bebedero", "Notificaciones de la app bebedero") // Create notification channel when the activity is created

        getMaxWaterTemperatureAndSetToSeekBarValue(seekBarWaterTemperature, seekBarTextViewWaterTemperature)

        getWaterQualityAndSetToSeekBarValue(seekBarWaterQuality, seekBarTextViewWaterQuality)

        listenForChangesOnWaterTemperatureMaxAndValues()

        listenForChangesOnWaterLevelMinAndValues()

        listenForChangesOnWaterQualityValues()

        listenToChangesOnSeekBarProgressWaterTemperature(seekBarWaterTemperature, seekBarTextViewWaterTemperature)

        listenToChangesOnSeekBarProgressWaterQuality(seekBarWaterQuality, seekBarTextViewWaterQuality)

        listenForFillWatererChanges()
    }

    private fun setupMainView() {
        setContentView(R.layout.activity_main)
        drawerLayout = findViewById(R.id.drawer_layout)
        navView = findViewById(R.id.nav_view)
        menuButton = findViewById(R.id.menuButton)
        fillButton = findViewById(R.id.buttonFill)
        seekBarWaterTemperature = findViewById(R.id.seekBarWaterTemperature) // Reference to SeekBar value
        seekBarTextViewWaterTemperature = findViewById(R.id.seekBarValueTextViewWaterTemperature) // Reference to SeekBar text
        seekBarWaterQuality = findViewById(R.id.seekBarWaterQuality)
        seekBarTextViewWaterQuality = findViewById(R.id.seekBarValueTextViewQuality)

        fillButton.setOnClickListener {
            fillButton.isEnabled = false
            firebaseDatabaseInterface.setFillWaterer(1)
        }
    }

    private fun listenForFillWatererChanges() {
        firebaseDatabaseInterface.getFillWaterer { fillWatererValue ->
            if (fillWatererValue == 1) {
                Log.d("MainActivity", "El bebedero está siendo llenado")
            } else {
                Log.d("MainActivity", "El bebedero no está siendo llenado")
                fillButton.isEnabled = true
                userNotifiedAboutWaterTemperature = false
                userNotifiedAboutWaterQuality = false
                userNotifiedAboutWaterLevel = false
            }
        }
    }

    private fun navigationDrawerSetup() {
        drawerLayout.setDrawerLockMode(DrawerLayout.LOCK_MODE_LOCKED_CLOSED) // Disable navigation drawer sliding

        supportActionBar?.setDisplayHomeAsUpEnabled(true)
        supportActionBar?.setHomeAsUpIndicator(R.drawable.ic_menu) // Navigation drawer icon

        // Open navigation drawer
        menuButton.setOnClickListener {
            if (drawerLayout.isDrawerOpen(navView)) {
                drawerLayout.closeDrawer(navView)
            } else {
                drawerLayout.openDrawer(navView)
            }
        }

        // Open navigation drawer elements
        navView.setNavigationItemSelectedListener { menuItem ->
            when (menuItem.itemId) {
                R.id.nav_temperature -> {
                    val intent = Intent(this, MyTemperatureHistoryActivity::class.java)
                    startActivity(intent)
                    true
                }
                R.id.nav_level -> {
                    val intent = Intent(this, MyLevelHistoryActivity::class.java)
                    startActivity(intent)
                    true
                }
                else -> false
            }
        }
    }

    private fun initWaterTemperatureViews() {
        waterTemperatureView = findViewById(R.id.cardViewTemperature)
        progressBarWaterTemperature = waterTemperatureView.findViewById(R.id.circularProgressBar)
        progressBarWaterTemperatureText = waterTemperatureView.findViewById(R.id.percentageTextView)
        textViewWaterTemperature = findViewById(R.id.textViewWaterTemperature)
    }

    private fun initWaterLevelViews() {
        waterLevelView = findViewById(R.id.cardViewWaterLevel)
        textViewWaterLevel = findViewById(R.id.TextViewWaterLevel)
    }

    private fun initWaterQualityViews() {
        waterQualityView = findViewById(R.id.cardViewWaterQuality)
        textViewWaterQuality = findViewById(R.id.TextViewWaterQuality)
    }

    private fun getMaxWaterTemperatureAndSetToSeekBarValue(seekBar: SeekBar, textView: TextView) {
        firebaseDatabaseInterface.getMaxWaterTemperature { maxTemperature ->
            seekBar.progress = maxTemperature.toInt()
            textView.text = maxTemperature.toInt().toString()
        }
    }

    private fun getWaterQualityAndSetToSeekBarValue(seekBar: SeekBar, textView: TextView) {
        firebaseDatabaseInterface.getWaterMaxDaysWithoutFill { maxDaysWithoutFill ->
            seekBar.progress = maxDaysWithoutFill
            textView.text = maxDaysWithoutFill.toString()
        }
    }

    private fun listenForChangesOnWaterTemperatureMaxAndValues() {
        firebaseDatabaseInterface.getMaxWaterTemperature { temperature ->
            maxTemperature = temperature
            userNotifiedAboutWaterTemperature = false
            updateWaterTemperatureValues()
        }

        firebaseDatabaseInterface.getWaterTemperatures { _ ->
            updateWaterTemperatureValues()
        }
    }

    private fun listenForChangesOnWaterLevelMinAndValues() {
        firebaseDatabaseInterface.getMinWaterLevel { waterLevel ->
            minWaterLevel = waterLevel
            updateWaterLevelValues()
        }

        firebaseDatabaseInterface.getWaterLevels { _ ->
            updateWaterLevelValues()
        }
    }

    private fun listenForChangesOnWaterQualityValues() {
        firebaseDatabaseInterface.getWaterQuality { quality ->
            waterQuality = quality
            updateWaterQuality()
        }

        firebaseDatabaseInterface.getWaterMaxDaysWithoutFill { maxDays ->
            maxDaysWithoutFill = maxDays
            if (maxDaysWithoutFill != qualityLectureError) {
                userNotifiedAboutWaterQuality = false
                updateWaterQuality()
            }
            else {
                showErrorWaterQualityData()
            }
        }

        firebaseDatabaseInterface.getWaterMedDaysWithoutFill { medDays ->
            medDaysWithoutFill = medDays
            if (medDaysWithoutFill != qualityLectureError) {
                updateWaterQuality()
            }
            else {
                showErrorWaterQualityData()
            }
        }
    }

    private fun listenToChangesOnSeekBarProgressWaterTemperature(seekBar: SeekBar, progressTextView: TextView) {
        seekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean) {
                if (fromUser) {
                    progressTextView.text = progress.toString() // Update TextView text

                    firebaseDatabaseInterface.setMaxWaterTemperature(progress.toDouble()) // Update value on Firebase
                }
            }

            override fun onStartTrackingTouch(seekBar: SeekBar) { } // Method required but not used

            override fun onStopTrackingTouch(seekBar: SeekBar) { } // Method required but not used
        })
    }

    private fun listenToChangesOnSeekBarProgressWaterQuality(seekBar: SeekBar, progressTextView: TextView) {
        seekBar.setOnSeekBarChangeListener(object : SeekBar.OnSeekBarChangeListener {
            override fun onProgressChanged(seekBar: SeekBar, progress: Int, fromUser: Boolean) {
                if (fromUser) {
                    progressTextView.text = progress.toString() // Update TextView text

                    firebaseDatabaseInterface.setWaterMaxDaysWithoutFill(progress) // Update value on Firebase
                    val auxMedDaysWithoutFill: Int = progress/2
                    if (auxMedDaysWithoutFill >= 1){firebaseDatabaseInterface.setWaterMedDaysWithoutFill(progress/2)}
                    else {firebaseDatabaseInterface.setWaterMedDaysWithoutFill(1)}
                }
            }

            override fun onStartTrackingTouch(seekBar: SeekBar) { } // Method required but not used

            override fun onStopTrackingTouch(seekBar: SeekBar) { } // Method required but not used
        })
    }

    @SuppressLint("SetTextI18n")
    private fun updateWaterTemperatureValues() {
        firebaseDatabaseInterface.getWaterTemperatures { temperatures ->
            if (temperatures.isNotEmpty()) {
                val averageTemperature = temperatures.average()
                Log.d("MainActivity", "Temperatura promedio: $averageTemperature")

                // Show the average value, a backslash and then the maximum temperature in the TextView
                textViewWaterTemperature.text = "${averageTemperature.toInt()}ºC / ${maxTemperature.toInt()}ºC"

                // Calculate percentage and update progressBar
                var progressBarPercentage = ((averageTemperature * 100) / maxTemperature).toInt()
                Log.d("MainActivity", "Porcentaje temperatura: $progressBarPercentage")
                if (progressBarPercentage > 100) {
                    progressBarPercentage = 100
                }

                progressBarWaterTemperature.progress = progressBarPercentage
                progressBarWaterTemperatureText.text = "${progressBarPercentage}%"

                if (progressBarPercentage >= 90 && !userNotifiedAboutWaterTemperature) {
                    notificationHandler.showNotificationTemperature("TEMPERATURA BEBEDERO", "Revisar bebedero", "La temperatura del agua se encuentra en estado crítico. Considere cambiar el agua del bebedero.")
                    notificationHandler.showSummaryNotification()
                    userNotifiedAboutWaterTemperature = true
                }
            } else {
                Log.d("MainActivity", "No temperature data available")
                progressBarWaterTemperatureText.text = "NaN"
                progressBarWaterTemperature.progress = 0
                textViewWaterTemperature.text = "Recopilando datos de temperatura del agua..."
            }
        }
    }

    @SuppressLint("SetTextI18n")
    private fun updateWaterLevelValues() {
        firebaseDatabaseInterface.getWaterLevels { waterLevel ->
            if (waterLevel.isNotEmpty()) {
                val averageWaterLevel = waterLevel.average() // Get average using water levels
                Log.d("MainActivity", "Nivel promedio agua: $averageWaterLevel")

                val waterLevelMessage = when {
                    averageWaterLevel <= minWaterLevel -> "Rellenar bebedero"
                    else -> "Nivel de agua: estable"
                }
                textViewWaterLevel.text = waterLevelMessage

                // Check if water level is critical and show notification
                if ((averageWaterLevel < minWaterLevel) && !userNotifiedAboutWaterLevel) {
                    userNotifiedAboutWaterLevel = true
                    notificationHandler.showNotificationWaterLevel(
                        "NIVEL DE AGUA BAJO",
                        "Nivel de agua bajo",
                        "El nivel de agua en el bebedero es bajo. Por favor, considere rellenar el bebedero."
                    )
                }

            } else {
                Log.d("MainActivity", "No water level data available")
                textViewWaterLevel.text = "Recopilando datos del nivel de agua..."
            }
        }
    }

    @SuppressLint("SetTextI18n")
    private fun updateWaterQuality() {
        if (waterQuality != qualityLectureError) {
            val waterQualityMessage: String = when {
                waterQuality < medDaysWithoutFill  -> "Calidad del agua: excelente"
                waterQuality < maxDaysWithoutFill -> "Calidad del agua: buena"
                else -> {
                    if (!userNotifiedAboutWaterQuality) {
                        userNotifiedAboutWaterQuality = true
                        notificationHandler.showNotificationQuality("CALIDAD DEL AGUA", "Revisar bebedero", "La calidad del agua no es buena. Considere cambiar el agua del bebedero.")
                        notificationHandler.showSummaryNotification()
                    }
                    "Cambiar agua del bebedero"
                }
            }

            textViewWaterQuality.text = waterQualityMessage
        }

        else {
            showErrorWaterQualityData()
        }
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        if (item.itemId == android.R.id.home) {
            if (drawerLayout.isDrawerOpen(navView)) {
                drawerLayout.closeDrawer(navView)
            } else {
                drawerLayout.openDrawer(navView)
            }
            return true
        }
        return super.onOptionsItemSelected(item)
    }

    @SuppressLint("SetTextI18n")
    private fun showErrorWaterQualityData() {
        Log.d("MainActivity", "No water quality data available")
        textViewWaterQuality.text = "Recopilando datos de la calidad del agua..."
    }
}