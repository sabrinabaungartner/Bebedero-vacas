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

    private lateinit var progressBar: ProgressBar
    private lateinit var maxTemperatureTextView: TextView
    private lateinit var averageTemperatureTextView: TextView
    private lateinit var percentageTextView: TextView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Inicializar las vistas
        progressBar = findViewById(R.id.circularProgressBar)
        maxTemperatureTextView = findViewById(R.id.maxTemperatureTextView)
        averageTemperatureTextView = findViewById(R.id.averageTemperatureTextView)
        percentageTextView = findViewById(R.id.percentageTextView)

        // Escuchar cambios en los valores de la base de datos y actualizar la interfaz de usuario
        firebaseDatabaseInterface.getMaxWaterTemperature { temperature ->
            maxTemperature = temperature
            updateWaterTemperatureValues()
        }

        firebaseDatabaseInterface.getWaterTemperatures { _ ->
            updateWaterTemperatureValues()
        }
    }

    @SuppressLint("SetTextI18n")
    private fun updateWaterTemperatureValues() {
        // Obtener la temperatura máxima y establecerla en el TextView correspondiente
        maxTemperatureTextView.text = "Máxima: $maxTemperature °C"
        Log.d("MainActivity", "Temperatura máxima: $maxTemperature")

        //progressBar.max = maxTemperature.toInt()

        // Obtener las temperaturas y calcular el promedio
        firebaseDatabaseInterface.getWaterTemperatures { temperatures ->
            if (temperatures.isNotEmpty()) {
                val averageTemperature = temperatures.average()
                Log.d("MainActivity", "Temperatura promedio: $averageTemperature")
                averageTemperatureTextView.text = "Promedio: $averageTemperature °C"

                // Mostrar el valor promedio, una barra invertida y luego la temperatura máxima en el TextView
                percentageTextView.text = "${averageTemperature.toInt()}ºC / ${maxTemperature.toInt()}ºC"

                // Calcular el porcentaje y actualizar el progreso del ProgressBar
                val progressBarPercentage = ((averageTemperature * 100) / maxTemperature).toInt()
                progressBar.progress = progressBarPercentage
            } else {
                Log.d("MainActivity", "No hay datos de temperatura disponibles")
            }
        }
    }
}