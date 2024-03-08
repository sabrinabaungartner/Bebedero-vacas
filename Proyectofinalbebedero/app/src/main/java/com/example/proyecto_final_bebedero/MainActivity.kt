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
    private lateinit var percentageTextView: TextView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        initTemperatureViews()

        // Escuchar cambios en los valores de la base de datos y actualizar la interfaz de usuario
        firebaseDatabaseInterface.getMaxWaterTemperature { temperature ->
            maxTemperature = temperature
            updateWaterTemperatureValues()
        }

        firebaseDatabaseInterface.getWaterTemperatures { _ ->
            updateWaterTemperatureValues()
        }
    }

    private fun initTemperatureViews() {
        progressBarWaterTemperature = findViewById(R.id.circularProgressBarWaterTemperature)
        progressBarWaterLevel = findViewById(R.id.circularProgressBarWaterLevel) // Inicializar la segunda ProgressBar
        percentageTextView = findViewById(R.id.percentageTextView)
    }

    @SuppressLint("SetTextI18n")
    private fun updateWaterTemperatureValues() {
        // Obtener la temperatura máxima y establecerla en el TextView correspondiente
        firebaseDatabaseInterface.getWaterTemperatures { temperatures ->
            if (temperatures.isNotEmpty()) {
                val averageTemperature = temperatures.average()
                Log.d("MainActivity", "Temperatura promedio: $averageTemperature")

                // Mostrar el valor promedio, una barra invertida y luego la temperatura máxima en el TextView
                percentageTextView.text = "${averageTemperature.toInt()}ºC / ${maxTemperature.toInt()}ºC"

                // Calcular el porcentaje y actualizar el progreso del ProgressBar
                val progressBarPercentage = ((averageTemperature * 100) / maxTemperature).toInt()
                Log.d("MainActivity", "Porcentaje: $progressBarPercentage")
                progressBarWaterTemperature.progress = progressBarPercentage
                progressBarWaterLevel.progress = progressBarPercentage // Actualizar la segunda ProgressBar
            } else {
                Log.d("MainActivity", "No hay datos de temperatura disponibles")
                percentageTextView.text = "NoN"
                progressBarWaterTemperature.progress = 0
                progressBarWaterLevel.progress = 0 // Actualizar la segunda ProgressBar
            }
        }
    }
}
