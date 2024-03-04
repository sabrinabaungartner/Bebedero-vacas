package com.example.proyecto_final_bebedero

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity

class MainActivity : AppCompatActivity() {
    private val firebaseDatabaseHandler = FirebaseDatabaseHandler()

    //Funcion que se llama cada vez que se lanza la pantalla
    override fun onCreate(savedInstanceState: Bundle?) {
        //Lo que se quiere hacer en el inicio se hace aca
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main) //Diseño que se aplica a la pantalla de la funcion fun, a la pantalla ppal

        firebaseDatabaseHandler.calcularPromedioTemperaturaAgua(
            onSuccessListener = { promedio ->
                Log.d("MainActivity", "El promedio de la temperatura del agua es: $promedio")
                // Mostrar el promedio en tu interfaz de usuario
            },
            onFailureListener = { error ->
                Log.e("MainActivity", "Error al calcular el promedio de temperatura del agua: $error")
            }
        )
    }
}