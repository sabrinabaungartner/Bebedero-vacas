package com.example.proyecto_final_bebedero

interface FirebaseDatabaseInterface {
    fun getMaxWaterTemperature(listener: (Double) -> Unit) //listener lambda function

    fun getWaterTemperatures(listener: (List<Double>) -> Unit) //listener lambda function

    fun getMinWaterLevel(listener: (Double) -> Unit) //listener lambda function

    fun getWaterLevels(listener: (List<Double>) -> Unit) //listener lambda function

    fun getWaterQuality(listener: (Int) -> Unit) //listener lambda function

    fun getWaterMaxDaysWithoutFill(listener: (Int) -> Unit) //listener lambda function

    fun getWaterMedDaysWithoutFill(listener: (Int) -> Unit) //listener lambda function

    fun setMaxWaterTemperature(value: Double)

    fun setWaterMaxDaysWithoutFill(value: Int)

    fun setWaterMedDaysWithoutFill(value: Int)

    fun setDateOfLastFilling()
}