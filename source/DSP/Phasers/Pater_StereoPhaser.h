class StereoPhaser : public PluginBase<StereoPhaser>
{
public:
    StereoPhaser();

    static void addParameters (Parameters& params);
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (AudioBuffer<float>& buffer) override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    AudioProcessorEditor* createEditor() override;

private:
    std::unique_ptr<MonoPhaser> phasers[2];
    AudioBuffer<float> noModBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StereoPhaser)
};

