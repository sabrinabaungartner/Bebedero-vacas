package com.example.proyecto_final_bebedero

import android.annotation.SuppressLint
import android.os.Bundle
import android.util.Log
import android.view.MenuItem
import android.widget.ImageButton
import android.widget.ProgressBar
import android.widget.TextView
import androidx.appcompat.app.ActionBarDrawerToggle
import androidx.appcompat.app.AppCompatActivity
import androidx.drawerlayout.widget.DrawerLayout
import com.google.android.material.navigation.NavigationView

class MainActivity : AppCompatActivity() {
    private val firebaseDatabaseInterface: FirebaseDatabaseInterface = FirebaseDatabaseHandler()
    private var maxTemperature: Double = 0.0
    private var maxWaterLevel: Double = 0.0
    private var waterQuality: Int = 1
    private var maxDaysWithoutFill = 1
    private var medDaysWithoutFill = 1
    private val qualityLectureError = 99

    private lateinit var progressBarWaterTemperature: ProgressBar
    private lateinit var progressBarWaterLevel: ProgressBar
    private lateinit var progressBarWaterQuality: ProgressBar

    private lateinit var percentageTextViewWaterTemperature: TextView
    private lateinit var textViewWaterLevel: TextView
    private lateinit var textViewWaterQuality: TextView

    private lateinit var drawerLayout: DrawerLayout
    private lateinit var navView: NavigationView
    private lateinit var menuButton: ImageButton

    @SuppressLint("SetTextI18n")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        drawerLayout = findViewById(R.id.drawer_layout)
        navView = findViewById(R.id.nav_view)
        menuButton = findViewById(R.id.menuButton)

        // Configurar el ActionBarDrawerToggle para sincronizar el DrawerLayout y el ActionBar
        //val toggle = ActionBarDrawerToggle(this, drawerLayout, R.string.navigation_drawer_open, R.string.navigation_drawer_close)
        //drawerLayout.addDrawerListener(toggle)
        //toggle.syncState()

        // Deshabilitar el deslizamiento del cajón de navegación
        drawerLayout.setDrawerLockMode(DrawerLayout.LOCK_MODE_LOCKED_CLOSED)

        supportActionBar?.setDisplayHomeAsUpEnabled(true)
        supportActionBar?.setHomeAsUpIndicator(R.drawable.ic_menu) // Navigation drawer icon

        // Manejar clics en el botón para abrir el cajón de navegación
        menuButton.setOnClickListener {
            if (drawerLayout.isDrawerOpen(navView)) {
                drawerLayout.closeDrawer(navView)
            } else {
                drawerLayout.openDrawer(navView)
            }
        }

        // Manejar clics en los elementos del menú de navegación
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

        initTemperatureViews()
        initWaterLevelViews()
        initWaterQualityViews()

        // Escuchar cambios en los valores de la base de datos y actualizar la interfaz de usuario
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
                Log.d("MainActivity", "No water quality data available")
                textViewWaterQuality.text = "NaN"
                progressBarWaterQuality.progress = 0
            }
        }

        firebaseDatabaseInterface.getWaterMaxDaysWithoutFill { maxDays ->
            maxDaysWithoutFill = maxDays
            if (maxDaysWithoutFill != qualityLectureError) {
                updateWaterQuality()
            }
            else {
                Log.d("MainActivity", "No water max quality data available")
                textViewWaterQuality.text = "NaN"
                progressBarWaterQuality.progress = 0
            }
        }

        firebaseDatabaseInterface.getWaterMedDaysWithoutFill { medDays ->
            medDaysWithoutFill = medDays
            if (medDaysWithoutFill != qualityLectureError) {
                updateWaterQuality()
            }
            else {
                Log.d("MainActivity", "No water med quality data available")
                textViewWaterQuality.text = "NaN"
                progressBarWaterQuality.progress = 0
            }
        }
    }

    private fun initTemperatureViews() {
        progressBarWaterTemperature = findViewById(R.id.circularProgressBarWaterTemperature)
        percentageTextViewWaterTemperature = findViewById(R.id.percentageTextViewWaterTemperature)
    }

    private fun initWaterLevelViews() {
        progressBarWaterLevel = findViewById(R.id.circularProgressBarWaterLevel)
        textViewWaterLevel = findViewById(R.id.TextViewWaterLevel)
    }

    private fun initWaterQualityViews() {
        progressBarWaterQuality = findViewById(R.id.circularProgressBarWaterQuality)
        textViewWaterQuality = findViewById(R.id.TextViewWaterQuality)
    }

    @SuppressLint("SetTextI18n")
    private fun updateWaterTemperatureValues() {
        // Obtener la temperatura máxima y establecerla en el TextView correspondiente
        firebaseDatabaseInterface.getWaterTemperatures { temperatures ->
            if (temperatures.isNotEmpty()) {
                val averageTemperature = temperatures.average()
                Log.d("MainActivity", "Temperatura promedio: $averageTemperature")

                // Show the average value, a backslash and then the maximum temperature in the TextView
                percentageTextViewWaterTemperature.text = "${averageTemperature.toInt()}ºC / ${maxTemperature.toInt()}ºC"

                // Calculate percentage and update progressBar
                val progressBarPercentage = ((averageTemperature * 100) / maxTemperature).toInt()
                Log.d("MainActivity", "Porcentaje temperatura: $progressBarPercentage")
                progressBarWaterTemperature.progress = progressBarPercentage
            } else {
                Log.d("MainActivity", "No temperature data available")
                percentageTextViewWaterTemperature.text = "NaN"
                progressBarWaterTemperature.progress = 0
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

                val waterLevelMessage = when {
                    progressBarPercentageWater < 34 -> "Nivel de agua: ok"
                    progressBarPercentageWater < 68 -> "Nivel de agua: estable"
                    else -> "Rellenar bebedero"
                }
                textViewWaterLevel.text = waterLevelMessage

            } else {
                Log.d("MainActivity", "No water level data available")
                textViewWaterLevel.text = "NaN"
                progressBarWaterLevel.progress = 0
            }
        }
    }

    private fun updateWaterQuality() {
        // Calculate percentage and update progressBar
        val progressBarWaterQualityValue = ((waterQuality * 100) / maxDaysWithoutFill)
        Log.d("MainActivity", "Porcentaje water quality: $progressBarWaterQualityValue")
        progressBarWaterQuality.progress = progressBarWaterQualityValue

        val waterQualityMessage = when {
            waterQuality < medDaysWithoutFill  -> "Calidad del agua: buena"
            waterQuality < maxDaysWithoutFill -> "Calidad del agua: media"
            else -> "Cambiar agua del bebedero"
        }
        textViewWaterQuality.text = waterQualityMessage.toString()
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
}
