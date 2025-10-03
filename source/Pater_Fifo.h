#pragma once

// This header defines a lock-free FIFO buffer utility.

#include <array>
#include <vector>
#include <type_traits>
#include <juce_audio_basics/juce_audio_basics.h>

namespace Fifo
{
template<typename T>
struct IsReferenceCountedObjectPtr : std::false_type { };

template<typename T>
struct IsReferenceCountedObjectPtr<juce::ReferenceCountedObjectPtr<T>> : std::true_type { };

template<typename T>
struct IsReferenceCountedArray : std::false_type { };

template<typename T>
struct IsReferenceCountedArray<juce::ReferenceCountedArray<T>> : std::true_type { };

template<typename T, size_t Size = 30>
struct Fifo
{
    void prepare(int numChannels, int numSamples)
    {
        static_assert( std::is_same_v<T, juce::AudioBuffer<float>>,
                      "prepare(numChannels, numSamples) should only be used when the Fifo is holding juce::AudioBuffer<float>");
        for( auto& buffer : buffers)
        {
            buffer.setSize(numChannels,
                           numSamples,
                           false,   //clear everything?
                           true,    //including the extra space?
                           true);   //avoid reallocating if you can?
            buffer.clear();
        }
    }

    void prepare(size_t numElements)
    {
        static_assert( std::is_same_v<T, std::vector<float>>,
                      "prepare(numElements) should only be used when the Fifo is holding std::vector<float>");
        for( auto& buffer : buffers )
        {
            buffer.clear();
            buffer.resize(numElements, 0);
        }
    }

    bool push(const T& t)
    {
        auto write = fifo.write(1);
        if( write.blockSize1 > 0 )
        {
            buffers[static_cast<size_t>(write.startIndex1)] = t;
            return true;
        }

        return false;
    }

    bool pull(T& t)
    {
        auto read = fifo.read(1);
        if( read.blockSize1 > 0 )
        {
            t = buffers[static_cast<size_t>(read.startIndex1)];
            return true;
        }

        return false;
    }

    bool exchange(T&& t)
    {
        auto read = fifo.read(1);
        if( read.blockSize1 > 0 )
        {
            auto idx = static_cast<size_t>(read.startIndex1);
            if constexpr( IsReferenceCountedObjectPtr<T>::value)
            {
                jassert(t.get() == nullptr);

                std::swap(t, buffers[idx]);
                jassert(buffers[idx].get() == nullptr );
            }
            else if constexpr( IsReferenceCountedArray<T>::value)
            {
                std::swap(buffers[idx], t);
                jassert( buffers[idx].size() == 0 );
            }
            else if constexpr (std::is_same_v<T, std::vector<float>>)
            {
                if( t.size() < buffers[idx].size() )
                {
                    t = buffers[idx]; //can't swap.  must copy.
                }
                else
                {
                    std::swap( t, buffers[idx] ); //ok to swap.
                }
            }
            else if constexpr( std::is_same_v<T, juce::AudioBuffer<float>> )
            {
                if( t.getNumSamples() < buffers[idx].getNumSamples() )
                {
                    t = buffers[idx]; //can't swap.  must copy
                }
                else
                {
                    std::swap( t, buffers[idx]); //ok to swap.
                }
            }
            else //T is something else.  blindly swap.  this leaves buffer[x] in a potentially invalid state that could allocate the next time it's used.
            {
                std::swap( t, buffers[idx]);
//                jassertfalse;
            }

            return true;
        }

        return false;
    }

    int getNumAvailableForReading() const
    {
        return fifo.getNumReady();
    }
private:
    std::array<T, Size> buffers;
    juce::AbstractFifo fifo {Size};
};

}

// Provide a project-wide namespace alias expected by other code.
// Now code can use either Fifo::Fifo<T, Size> or Pater_Fifo::Fifo<T, Size>.
namespace Pater_Fifo {
    template<typename T, size_t Size = 30>
    using Fifo = ::Fifo::Fifo<T, Size>;
}

