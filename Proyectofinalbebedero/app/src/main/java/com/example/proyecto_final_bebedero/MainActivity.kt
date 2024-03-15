package com.example.proyecto_final_bebedero

import android.annotation.SuppressLint
import android.os.Bundle
import android.util.Log
import android.view.MenuItem
import android.widget.ImageButton
import android.widget.ProgressBar
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity
import androidx.cardview.widget.CardView
import androidx.drawerlayout.widget.DrawerLayout
import com.google.android.material.navigation.NavigationView

class MainActivity : AppCompatActivity() {
    private val firebaseDatabaseInterface: FirebaseDatabaseInterface = FirebaseDatabaseHandler()
    private lateinit var notificationHandler: PushNotificationsInterface

    private var maxTemperature: Double = 0.0
    private var maxWaterLevel: Double = 0.0
    private var waterQuality: Int = 1
    private var maxDaysWithoutFill = 1
    private var medDaysWithoutFill = 1
    private val qualityLectureError = 99

    // Vars for water temperature
    private lateinit var waterTemperatureView: CardView
    private lateinit var progressBarWaterTemperature: ProgressBar
    private lateinit var progressBarWaterTemperatureText: TextView
    private lateinit var textViewWaterTemperature: TextView

    // Vars for water level
    private lateinit var waterLevelView: CardView
    private lateinit var progressBarWaterLevel: ProgressBar
    private lateinit var progressBarWaterLevelText: TextView
    private lateinit var textViewWaterLevel: TextView

    // Vars for water quality
    private lateinit var waterQualityView: CardView
    private lateinit var progressBarWaterQuality: ProgressBar
    private lateinit var progressBarWaterQualityText: TextView
    private lateinit var textViewWaterQuality: TextView

    private lateinit var drawerLayout: DrawerLayout
    private lateinit var navView: NavigationView
    private lateinit var menuButton: ImageButton

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setupMainView()

        navigationDrawerSetup()

        initWaterTemperatureViews()
        initWaterLevelViews()
        initWaterQualityViews()

        notificationHandler = PushNotificationsHandler(this) // Initialize the notification handler
        notificationHandler.createNotificationChannel("Bebedero", "Notificaciones de la app bebedero") // Create notification channel when the activity is created

        // Listen for changes to database values and update the user interface
        firebaseDatabaseInterface.getMaxWaterTemperature { temperature ->
            maxTemperature = temperature
            updateWaterTemperatureValues()
        }

        firebaseDatabaseInterface.getWaterTemperatures { _ ->
            updateWaterTemperatureValues()
        }

        firebaseDatabaseInterface.getMaxWaterLevel { waterLevel ->
            maxWaterLevel = waterLevel
            updateWaterLevelValues()
        }

        firebaseDatabaseInterface.getWaterLevels { _ ->
            updateWaterLevelValues()
        }

        firebaseDatabaseInterface.getWaterQuality { quality ->
            waterQuality = quality
            if (waterQuality != qualityLectureError) {
                updateWaterQuality()
            }
            else {
                showErrorWaterQualityData()
            }
        }

        firebaseDatabaseInterface.getWaterMaxDaysWithoutFill { maxDays ->
            maxDaysWithoutFill = maxDays
            if (maxDaysWithoutFill != qualityLectureError) {
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

    private fun setupMainView() {
        setContentView(R.layout.activity_main)
        drawerLayout = findViewById(R.id.drawer_layout)
        navView = findViewById(R.id.nav_view)
        menuButton = findViewById(R.id.menuButton)
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
                R.id.nav_home -> {
                    // Manejar la acción correspondiente al elemento del menú "Inicio"
                    // Por ejemplo, puedes abrir otra actividad o realizar alguna acción específica
                    true
                }
                // Agregar otros casos según sea necesario para manejar otros elementos del menú
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
        progressBarWaterLevel = waterLevelView.findViewById(R.id.circularProgressBar)
        progressBarWaterLevelText = waterLevelView.findViewById(R.id.percentageTextView)
        textViewWaterLevel = findViewById(R.id.TextViewWaterLevel)
    }

    private fun initWaterQualityViews() {
        waterQualityView = findViewById(R.id.cardViewWaterQuality)
        progressBarWaterQuality = waterQualityView.findViewById(R.id.circularProgressBar)
        progressBarWaterQualityText = waterQualityView.findViewById(R.id.percentageTextView)
        textViewWaterQuality = findViewById(R.id.TextViewWaterQuality)
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
                val progressBarPercentage = ((averageTemperature * 100) / maxTemperature).toInt()
                Log.d("MainActivity", "Porcentaje temperatura: $progressBarPercentage")
                progressBarWaterTemperature.progress = progressBarPercentage
                progressBarWaterTemperatureText.text = "${progressBarPercentage}%"

                if (progressBarPercentage >= 90) {
                    notificationHandler.showNotification("TEMPERATURA BEBEDERO", "La temperatura supera la máxima permitida")
                }
            } else {
                Log.d("MainActivity", "No temperature data available")
                progressBarWaterTemperatureText.text = "NaN"
                progressBarWaterTemperature.progress = 0
                textViewWaterTemperature.text = "Datos de temperatura no disponibles"
            }
        }
    }

    @SuppressLint("SetTextI18n")
    private fun updateWaterLevelValues() {
        firebaseDatabaseInterface.getWaterLevels { waterLevel ->
            if (waterLevel.isNotEmpty()) {
                val averageWaterLevel = waterLevel.average() // Get average using water levels
                Log.d("MainActivity", "Nivel promedio agua: $averageWaterLevel")

                // Calculate percentage and update progressBar
                val progressBarPercentageWater = ((averageWaterLevel * 100) / maxWaterLevel).toInt()
                Log.d("MainActivity", "Porcentaje water level: $progressBarPercentageWater")
                progressBarWaterLevel.progress = progressBarPercentageWater
                progressBarWaterLevelText.text = "${progressBarPercentageWater}%"

                val waterLevelMessage = when {
                    progressBarPercentageWater < 34 -> "Nivel de agua: excelente"
                    progressBarPercentageWater < 68 -> "Nivel de agua: estable"
                    else -> "Rellenar bebedero"
                }
                textViewWaterLevel.text = waterLevelMessage

            } else {
                Log.d("MainActivity", "No water level data available")
                textViewWaterLevel.text = "Datos del nivel de agua no disponibles"
                progressBarWaterLevel.progress = 0
                progressBarWaterLevelText.text = "Nan"
            }
        }
    }

    @SuppressLint("SetTextI18n")
    private fun updateWaterQuality() {
        // Calculate percentage and update progressBar
        val progressBarWaterQualityValue = ((waterQuality * 100) / maxDaysWithoutFill)
        Log.d("MainActivity", "Porcentaje water quality: $progressBarWaterQualityValue")
        progressBarWaterQuality.progress = progressBarWaterQualityValue
        progressBarWaterQualityText.text = "${progressBarWaterQualityValue}%"

        val waterQualityMessage = when {
            waterQuality < medDaysWithoutFill  -> "Calidad del agua: buena"
            waterQuality < maxDaysWithoutFill -> "Calidad del agua: media"
            else -> "Cambiar agua del bebedero"
        }
        textViewWaterQuality.text = waterQualityMessage
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
        Log.d("MainActivity", "No water med quality data available")
        textViewWaterQuality.text = "No hay datos de calidad del agua disponibles"
        progressBarWaterQuality.progress = 0
        progressBarWaterQualityText.text = "NaN"
    }
}