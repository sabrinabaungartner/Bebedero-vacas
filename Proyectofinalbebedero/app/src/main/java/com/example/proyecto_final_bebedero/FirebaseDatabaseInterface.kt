package com.example.proyecto_final_bebedero

interface FirebaseDatabaseInterface {
    fun addTemperatureMaxListener(listener: (Double) -> Unit)
    fun calcularPromedioTemperaturaAgua(onSuccessListener: (Double) -> Unit, onFailureListener: (String) -> Unit)
}