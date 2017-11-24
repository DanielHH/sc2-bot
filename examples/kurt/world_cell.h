

class WorldCell {
    
private:
    
    float mineral_amount;
    float gas_amount;
    float enemy_dps;
    float last_seen;
    
public:
    
    WordlCell(){
        
    }
    
    float GetMineralAmount(){
        return mineral_amount;
    }
    
    float GetGasAmount(){
        return gas_amount;
    }
    
    float GetEnemyDps(){
        return enemy_dps;
    }
    
    float GetLastSeen(){
        return last_seen;
    }
    
    void SetMineralAmount(float amount){
        mineral_amount = amount;
    }
    
    void SetGasAmount(float amount){
        gas_amount = amount;
    }
    
    void SetEnemyDps(float dps){
        enemy_dps = dps;
    }
    
    void SetLastSeen(float time){
        last_seen = time;
    }
};
