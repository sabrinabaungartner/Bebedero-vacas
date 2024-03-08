package com.example.proyecto_final_bebedero

interface FirebaseDatabaseInterface {
    /**fun getMaxWaterTemperature(listener: (Double) -> Unit) //listener lambda function

    //onSuccessListener and onFailureListener lambda functions
    fun calculateAverageWaterTemperature(onSuccessListener: (Double) -> Unit, onFailureListener: (String) -> Unit)**/

    fun getMaxWaterTemperature(listener: (Double) -> Unit) //listener lambda function

    fun getWaterTemperatures(listener: (List<Double>) -> Unit) //listener lambda function

    fun getMaxWaterLevel(listener: (Double) -> Unit) //listener lambda function

    fun getWaterLevels(listener: (List<Double>) -> Unit) //listener lambda function
}