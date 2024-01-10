#ifndef __ENCODER_HPP
#define __ENCODER_HPP

class EncoderABZ {
    public:
        EncoderABZ();

        void Init();
        
        void UpdateRPM(void);
        void UpdateEncoderUpperCount(void);
        void UpdateIndex(void);
        bool IsInitialized(void);
        uint32_t GetFullIndexCounts(void);
        uint16_t GetAngularPositionInCounts(void);
        uint16_t GetTotalAngularCounts();
        uint16_t GetRpm(void);
    private:
        bool isInitialized;
        void ABTimer_Encoder_Init_16bit(void);
        void Z_Init_32bit(void);
        void SysTick_Init(void);
        
        volatile uint16_t upperCounter;
        volatile uint32_t lastEncoderCount;

        //65,535 maximum rpm, should be plenty!
        volatile uint16_t rpm; 
        volatile uint32_t elapsedTime;
};

#endif // __ENCODER_HPP