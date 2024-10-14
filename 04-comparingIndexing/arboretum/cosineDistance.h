template <class DType>
class CosineDistance : public DistanceFunction<DType>
{
public:
    CosineDistance()
    {
    }

    /**
     * Returns the distance between 2 cities. This method is required by
     * stMetricEvaluator interface.
     *
     * @param obj1 Object 1.
     * @param obj2 Object 2.
     */
    double GetDistance(DType &obj1, DType &obj2)
    {
        return sqrt(GetDistance2(obj1, obj2));
    } // end GetDistance

    double getDistance(DType &obj1, DType &obj2)
    {
        return sqrt(GetDistance2(obj1, obj2));
    }

    /**
     * Returns the distance between 2 cities raised by the power of 2.
     * This method is required by stMetricEvaluator interface.
     *
     * @param obj1 Object 1.
     * @param obj2 Object 2.
     */
    double GetDistance2(DType &obj1, DType &obj2)
    {
        if (obj1.size() != obj2.size())
        {
            throw std::length_error("The feature vectors do not have the same size.");
        }

        double d = 0;
        double tmp;

        for (size_t i = 0; i < obj1.size(); i++)
        {
            d += obj1[i] * obj2[i];
        }

        // Statistic support
        this->updateDistanceCount();

        return (1. - d) * (1. - d);
    } // end GetDistance2

}; // end CosineDistance