package com.example.proyecto_final_bebedero

import android.annotation.SuppressLint
import android.os.Bundle
import android.util.Log
import android.widget.ProgressBar
import android.widget.TextView
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {
    private val firebaseDatabaseInterface: FirebaseDatabaseInterface = FirebaseDatabaseHandler()
    private var maxTemperature: Double = 0.0
    private var maxWaterLevel: Double = 0.0

    private lateinit var progressBarWaterTemperature: ProgressBar
    private lateinit var progressBarWaterLevel: ProgressBar
    private lateinit var percentageTextViewWaterTemperature: TextView
    private lateinit var percentageTextViewWaterLevel: TextView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        initTemperatureViews()
        initWaterLevelViews()

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
    }

    private fun initTemperatureViews() {
        progressBarWaterTemperature = findViewById(R.id.circularProgressBarWaterTemperature)
        percentageTextViewWaterTemperature = findViewById(R.id.percentageTextViewWaterTemperature)
    }

    private fun initWaterLevelViews() {
        progressBarWaterLevel = findViewById(R.id.circularProgressBarWaterLevel)
        percentageTextViewWaterLevel = findViewById(R.id.percentageTextViewWaterLevel)
    }

    @SuppressLint("SetTextI18n")
    private fun updateWaterTemperatureValues() {
        // Obtener la temperatura máxima y establecerla en el TextView correspondiente
        firebaseDatabaseInterface.getWaterTemperatures { temperatures ->
            if (temperatures.isNotEmpty()) {
                val averageTemperature = temperatures.average()
                Log.d("MainActivity", "Temperatura promedio: $averageTemperature")

                // Mostrar el valor promedio, una barra invertida y luego la temperatura máxima en el TextView
                percentageTextViewWaterTemperature.text = "${averageTemperature.toInt()}ºC / ${maxTemperature.toInt()}ºC"

                // Calcular el porcentaje y actualizar el progreso del ProgressBar
                val progressBarPercentage = ((averageTemperature * 100) / maxTemperature).toInt()
                Log.d("MainActivity", "Porcentaje temperatura: $progressBarPercentage")
                progressBarWaterTemperature.progress = progressBarPercentage
            } else {
                Log.d("MainActivity", "No hay datos de temperatura disponibles")
                percentageTextViewWaterTemperature.text = "NaN"
                progressBarWaterTemperature.progress = 0
            }
        }
    }

    @SuppressLint("SetTextI18n")
    private fun updateWaterLevelValues() {
        // Obtener la temperatura máxima y establecerla en el TextView correspondiente
        firebaseDatabaseInterface.getWaterLevels { waterLevel ->
            if (waterLevel.isNotEmpty()) {
                val averageWaterLevel = waterLevel.average()
                Log.d("MainActivity", "Nivel promedio agua: $averageWaterLevel")

                // Mostrar el valor promedio, una barra invertida y luego la temperatura máxima en el TextView
                percentageTextViewWaterLevel.text = "Nivel de agua ${averageWaterLevel.toInt()} / ${maxWaterLevel.toInt()}"

                // Calcular el porcentaje y actualizar el progreso del ProgressBar
                val progressBarPercentageWater = ((averageWaterLevel * 100) / maxWaterLevel).toInt()
                Log.d("MainActivity", "Porcentaje water level: $progressBarPercentageWater")
                progressBarWaterLevel.progress = progressBarPercentageWater
            } else {
                Log.d("MainActivity", "No hay datos de temperatura disponibles")
                percentageTextViewWaterLevel.text = "NaN"
                progressBarWaterLevel.progress = 0
            }
        }
    }
}
