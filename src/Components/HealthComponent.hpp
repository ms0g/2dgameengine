#pragma once

struct HealthComponent {
    int healthPercentage;

    explicit HealthComponent(int healthPercentage = 0) {
        this->healthPercentage = healthPercentage;
    }
};
