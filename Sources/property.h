#ifndef PROPERTY_TEMLATE_H
#define PROPERTY_TEMLATE_H

#include <string>
#include <atomic>

class PropertyBase
{
public:
    virtual void SetFromString (const std::string &s) = 0;
    virtual std::string GetString() const = 0;
};

template <typename T> class Property: public PropertyBase
{
    public:
        Property(const T &i) { Set(i); }
        virtual void Set (const T &i) { value = i; }
        virtual T Get() const { return value; }

        void SetFromString (const std::string &s) override { FromString(s); }
        std::string GetString() const override { return ToString(); }
    private:
        std::string ToString() const;
        void FromString(const std::string &s);
        T value;
};

template <typename T> class PropertyReadOnly: public Property<T>
{
    public:
        PropertyReadOnly(const T &i): Property<T>(i){}

    private:
        using Property<T>::Set;
        using Property<T>::SetFromString;
};

template <typename T> class PropertyAtomic: public Property<T>
{
public:
    PropertyAtomic(const T &i): Property<T>(i){ Set(i); }
    void Set (const T &i) override { value.store(i); }
    T Get() const override { return value.load(); }
private:
    std::atomic<T> value;
};

#endif // PROPERTY_H
