package com.example.proyecto_final_bebedero

import android.os.Bundle
import android.util.Log
import android.widget.ProgressBar
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {
    private val firebaseDatabaseInterface: FirebaseDatabaseInterface = FirebaseDatabaseHandler()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        val progressBar = findViewById<ProgressBar>(R.id.circularProgressBar)

        // Agrega el listener para la temperatura máxima
        firebaseDatabaseInterface.addTemperatureMaxListener { temperaturaMaxima ->
            progressBar.max = temperaturaMaxima.toInt()
            Log.d("MainActivity", "La temp maxima es: ${temperaturaMaxima.toInt()}")
        }

        // Calcula el promedio de la temperatura del agua
        firebaseDatabaseInterface.calcularPromedioTemperaturaAgua(
            onSuccessListener = { promedio ->
                Log.d("MainActivity", "El promedio de la temperatura del agua es: $promedio")
                val progreso = promedio.toInt()
                progressBar.progress = progreso
            },
            onFailureListener = { error ->
                Log.e("MainActivity", "Error al calcular el promedio de temperatura del agua: $error")
            }
        )
    }
}