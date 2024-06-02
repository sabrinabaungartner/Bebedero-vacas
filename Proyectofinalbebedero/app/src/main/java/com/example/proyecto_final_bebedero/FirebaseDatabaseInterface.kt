package com.example.proyecto_final_bebedero

interface FirebaseDatabaseInterface {
    fun getMaxWaterTemperature(listener: (Double) -> Unit) //listener lambda function

    fun getWaterTemperatures(listener: (List<Double>) -> Unit) //listener lambda function

    fun getWaterTemperaturesSortedByDate(listener: (Map<String, List<Double>>) -> Unit) //listener lambda function

    fun getWaterLevelsSortedByDate(listener: (Map<String, List<Double>>) -> Unit) //listener lambda function

    fun getMinWaterLevel(listener: (Int) -> Unit) //listener lambda function

    fun getWaterLevels(listener: (List<Double>) -> Unit) //listener lambda function

    fun getWaterQuality(listener: (Int) -> Unit) //listener lambda function

    fun getWaterMaxDaysWithoutFill(listener: (Int) -> Unit) //listener lambda function

    fun getWaterMedDaysWithoutFill(listener: (Int) -> Unit) //listener lambda function

    fun setMaxWaterTemperature(value: Double)

    fun setWaterMaxDaysWithoutFill(value: Int)

    fun setWaterMedDaysWithoutFill(value: Int)

    fun setFillWaterer(value: Int)

    fun getFillWaterer(listener: (Int) -> Unit)
}