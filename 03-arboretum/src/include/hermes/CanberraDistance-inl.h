/* Copyright 2003-2017 GBDI-ICMC-USP <caetano@icmc.usp.br>
* 
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/**
* Constructor.
*/
template <class ObjectType>
CanberraDistance<ObjectType>::CanberraDistance(){
}

/**
* Destructor.
*/
template <class ObjectType>
CanberraDistance<ObjectType>::~CanberraDistance(){
}

/**
* @deprecated Use getDistance(ObjectType &obj1, ObjectType &obj2) instead.
*
* @copydoc getDistance(ObjectType &obj1, ObjectType &obj2) .
*/
template <class ObjectType>
double CanberraDistance<ObjectType>::GetDistance(ObjectType &obj1, ObjectType &obj2) throw (std::length_error){

    return getDistance(obj1, obj2);
}

/**
* To make this computations both feature vectors should have the same size().
*
* @param obj1: The first feature vector.
* @param obj2: The second feature vector.
* @throw Exception If the computation is not possible.
* @return The distance between feature vector 1 and feature vector 2.
*/
template <class ObjectType>
double CanberraDistance<ObjectType>::getDistance(ObjectType &obj1, ObjectType &obj2) throw (std::length_error){

    if (obj1.size() != obj2.size())
        throw std::length_error("The feature vectors do not have the same size.");

    double d = 0;
    double tmp;
    double ai, bi, mai, mbi;

    ai = bi = 0.0;

    for (size_t i = 0; i < obj1.size(); i++){

        ai = obj1[i];
        bi = obj2[i];

        mai = sqrt(pow(ai, 2.0));
        mbi = sqrt(pow(bi, 2.0));

        double den = mai+mbi;
        if (den == 0.0){
            den = 1.0;
        }

        tmp = (sqrt(pow((ai - bi), 2.0)))/den;
        d = d + tmp;
    }

    // Statistic support
    this->updateDistanceCount();

    return d;
}
