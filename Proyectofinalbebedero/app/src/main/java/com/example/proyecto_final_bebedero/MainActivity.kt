package com.example.proyecto_final_bebedero

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle

class MainActivity : AppCompatActivity() {
    //Funcion que se llama cada vez que se lanza la pantalla
    override fun onCreate(savedInstanceState: Bundle?) {
        //Lo que se quiere hacer en el inicio se hace aca
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main) //Diseño que se aplica a la pantalla de la funcion fun, a la pantalla ppal
    }
}