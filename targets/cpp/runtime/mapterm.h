// Copyright (c) 2016 IBM Corporation.
#ifndef _MAPTERM
#define _MAPTERM

// DO NOT INCLUDE DIRECTLY. INSTEAD INCLUDE term.h

#include <unordered_map>
#include <compat.h>
#include <iowrapper.h>
#include <set>

// Forward declarations
template<typename V>
class Option;

template<typename V>
class List;

template<typename a> ::Option<a>& newNONE(tosca::Context& ctx);
template<typename a> ::Option<a>& newSOME(tosca::Context& ctx, a& param);
template<typename a> ::List<a>& newNil(tosca::Context& ctx);
template<typename a> ::List<a>& newCons(tosca::Context& ctx, a& value, List<a>& next);


namespace tosca {

    // Forward declarations
    template<typename K, typename V> class MapTerm;
    template<typename K, typename V> MapTerm<K, V>& newMapTerm();
    template<typename T> T& NewRef(T&);

    // MapTerm type definition
    template<typename K, typename V>
    class MapTerm: public Term
    {
    public:
        virtual ~MapTerm()
        {
        }

        /**
         * Creates a new map reference which inherits all properties in this
         * instance.
         *
         * This reference is consumed.
         *
         * @return A new non-shared map.
         */
        virtual MapTerm<K, V>& extend()
        {
            throw new std::runtime_error("");
        }

        /**
         * Add key-value pair to map.
         *
         * @param key term. The reference is used.
         * @param value the associated term value. The reference is used.
         */
        virtual void putValue(Context& ctx, K& key, V& value)
        {
            throw new std::runtime_error("");
        }

        /**
         * Get value corresponding to given key
         * @param key
         * @return An new reference to an optional typed term.
         */
        virtual Option<V>& getValue(Context& ctx, K& key)
        {
            throw new std::runtime_error("");
        }

        /**
         * Put all entries in the given map into this map
         * @param map
         */
        virtual void putAll(MapTerm<K, V> map)
        {
            throw new std::runtime_error("");
        }

        /**
         * Gets map values
         * @param context
         * @return
         */
        virtual List<V>& values(Context& ctx)
        {
            throw new std::runtime_error("");
        }

        /**
         * Gets map keys
         * @param context
         * @return
         */
        virtual List<K>& keys(Context& ctx)
        {
            throw new std::runtime_error("");
        }

        /**
         * @return true when this map is empty
         */
        virtual bool isEmpty()
        {
            throw new std::runtime_error("");
        }

        /**
         * @return true when this map contains an entry for the given key
         */
        virtual bool containsKey(K key)
        {
            throw new std::runtime_error("");
        }

        /**
         * @return true when this map contains an entry for the given key, including variables.
         */
        virtual bool contains(Term key)
        {
            throw new std::runtime_error("");
        }

        /*
         * Make new map term.
         */
        static Term& MakeTerm(Context& ctx, const std::string& symbol)
        {
            return newMapTerm<K, V>();
        }

        // Overrides

        const std::string& Symbol() const
        {
            // TODO: map have no symbol.
            static const std::string emptymap("{}");
            return emptymap;
        }

        bool IsMap() const
        {
            return true;
        }

    };


    // MapTerm value
    template<typename K, typename V>
    class CMapTerm: public MapTerm<K, V>
    {
    public:
        CMapTerm() : parent(Optional<CMapTerm>::nullopt)
        {
        }

        virtual ~CMapTerm()
        {
            if (parent)
                parent.value().Release();

            for (auto iter = map.begin(); iter != map.end(); iter ++)
            {
                iter->first->Release();
                iter->second->Release();
            }
        }

        Term& Copy(Context& ctx)
        {
            return newMapTerm<K, V>();
        }

        MapTerm<K, V>& extend()
        {
            static const bool mapreuse = getenv("nomapreuse") == 0;
            if (mapreuse && this->refcount == 1)
                return *this;

            this->AddRef();
            CMapTerm<K, V>& extended = *(new CMapTerm<K, V>(*this));
            return extended;
        }

        void putValue(Context& ctx, K& key, V& value)
        {
            auto search = map.find(&key);
            const bool found = search != map.end();
            if (found)
                search->second->Release();

            map[&key] = &value;

            if (found)
                key.Release();
        }

        Option<V>& getValue(Context& ctx, K& key)
        {
            Optional<Term> ovalue = MapGetValue(ctx, key);
            if (ovalue)
                return newSOME<V>(ctx, dynamic_cast<V&>(ovalue.value()));
            return newNONE<V>(ctx);
        }

        Optional<Term> MapGetValue(Context& ctx, Term& key) const
        {
            auto search = map.find(&dynamic_cast<K&>(key));
            if (search == map.end())
            {
                if (parent)
                    return parent.value().MapGetValue(ctx, key);

                return Optional<Term>::nullopt;
            }
            return make_optional<Term>(tosca::NewRef(*search->second));
        }

        void MapPutValue(Context& ctx, Term& key, Term& value)
        {
            putValue(ctx, dynamic_cast<K&>(key), dynamic_cast<V&>(value));
        }

        void putAll(MapTerm<K, V> map)
        {
            throw new std::runtime_error("");
        }

        List<V>& values(Context& ctx)
        {
            CMapTerm<K, V>* cmap = this;
            List<V>* result = &newNil<V>(ctx);
            while (true)
            {
                for (auto it = cmap->map.begin(); it != cmap->map.end(); it ++)
                {
                    it->second->AddRef();
                    result = &newCons<V>(ctx, *it->second, *result);
                }
                if (!cmap->parent)
                    break;

                cmap = &cmap->parent.value();
            }
            return *result;
        }

        List<K>& keys(Context& ctx)
        {
            CMapTerm<K, V>* cmap = this;
            List<K>* result = &newNil<K>(ctx);
            while (true)
            {
                for (auto it = cmap->map.begin(); it != cmap->map.end(); it ++)
                {
                    it->first->AddRef();
                    result = &newCons<K>(ctx, *it->first, *result);
                }
                if (!cmap->parent)
                    break;

                cmap = &cmap->parent.value();
            }
            return *result;
        }

        void MapKeys(std::set<Term*>& keys) const
        {
            const CMapTerm<K, V>* cmap = this;
            while (true)
            {
                for (auto it = cmap->map.begin(); it != cmap->map.end(); it ++)
                {
                    it->first->AddRef();
                    keys.insert(it->first);
                }
                if (!cmap->parent)
                    break;

                cmap = &cmap->parent.value();
            }
        }

        bool isEmpty()
        {
            return map.empty() && (parent) ? parent.value().isEmpty() : true;
        }

        bool containsKey(K key)
        {
            throw new std::runtime_error("");
        }

        bool contains(Term key)
        {
            throw new std::runtime_error("");
        }

        bool DeepEquals(const Term& rhs, std::unordered_map<Variable*, Variable*>& varmap) const
        {
            // implement equality modulo variable and map.
            return true;
        }

        Term& Substitute(tosca::Context& ctx, std::unordered_map<Variable*, Term*>& substitutes)
        {
            MapTerm<K, V>& copy = newMapTerm<K, V>();
            CMapTerm<K, V>* cmap = this;
            while (true)
            {
                for (auto it = cmap->map.begin(); it != cmap->map.end(); it ++)
                {
                    it->first->AddRef();
                    it->second->AddRef();
                    copy.putValue(ctx, *it->first, dynamic_cast<V&>(it->second->Substitute(ctx, substitutes)));
                }
                if (!cmap->parent)
                    break;

                 cmap = &cmap->parent.value();
            }
            return copy;
        }

        void Print(IOWrapper& out, int count, bool indent)
        {
            CMapTerm<K, V>* cmap = this;

            out.Write('\n');
            if (indent)
                out.Indent(count);
            out.Write('{');
            count += 2;
            bool first = true;
            while (true)
            {
                for (auto it = cmap->map.begin(); it != cmap->map.end(); it ++)
                {
                    if (!first)
                        out.Write(',');
                    else
                        first = false;
                    out.Write('\n');
                    if (indent)
                        out.Indent(count);
                    it->first->Print(out, count, indent);
                    out.Write(':');
                    it->second->Print(out, count + 2, indent);
                }
                if (!cmap->parent)
                    break;

                cmap = &cmap->parent.value();
            }
            count -= 2;
            if (!isEmpty())
            {
                out.Write('\n');
                if (indent)
                    out.Indent(count);
            }
            out.Write('}');
        }

    protected:
        std::unordered_map<K*, V*> map;

        // Extended map
        Optional<CMapTerm> parent;

        CMapTerm(CMapTerm& parent): parent(make_optional<CMapTerm>(parent))
        {}
    };

    // Construction
    template<typename K, typename V>
    MapTerm<K, V>& newMapTerm()
    {
        return *(new CMapTerm<K, V>());
    }

}

#endif
