// EPOS-- ARMv8 MMU Mediator Declarations

#ifndef __armv8_mmu_h
#define __armv8_mmu_h

#include <system/memory_map.h>
#include <utility/string.h>
#include <utility/list.h>
#include <utility/debug.h>
#include <architecture/cpu.h>
#include <architecture/mmu.h>

__BEGIN_SYS

class MMU: public MMU_Common<0, 0, 0>
{
    friend class CPU;

private:
    typedef Grouping_List<unsigned int> List;

    static const bool colorful = Traits<MMU>::colorful;
    static const unsigned int COLORS = Traits<MMU>::COLORS;
    static const unsigned int MEM_BASE = Memory_Map::RAM_BASE;
    static const unsigned int APP_LOW = Memory_Map::APP_LOW;
    static const unsigned int PHY_MEM = Memory_Map::PHY_MEM;

public:
    // Page Flags
    typedef MMU_Common<0, 0, 0>::Flags Page_Flags;

    // Page_Table
    class Page_Table {};

    // Chunk (for Segment)
    class Chunk
    {
    public:
        Chunk() {}
        Chunk(unsigned int bytes, Flags flags): _phy_addr(alloc(bytes)), _bytes(bytes), _flags(flags) {}
        Chunk(Phy_Addr phy_addr, unsigned int bytes, Flags flags): _phy_addr(phy_addr), _bytes(bytes), _flags(flags) {}

        ~Chunk() { free(_phy_addr, _bytes); }

        unsigned int pts() const { return 0; }
        Flags flags() const { return _flags; }
        Page_Table * pt() const { return 0; }
        unsigned int size() const { return _bytes; }
        Phy_Addr phy_address() const { return _phy_addr; } // always CT
        int resize(unsigned int amount) { return 0; } // no resize in CT

    private:
        Phy_Addr _phy_addr;
        unsigned int _bytes;
        Page_Flags _flags;
    };

    // Page Directory
    typedef Page_Table Page_Directory;

    // Directory (for Address_Space)
    class Directory
    {
    public:
        Directory() {}
        Directory(Page_Directory * pd) {}

        Page_Table * pd() const { return 0; }

        void activate() {}

        Log_Addr attach(const Chunk & chunk) { return chunk.phy_address(); }
        Log_Addr attach(const Chunk & chunk, Log_Addr addr) { return (addr == chunk.phy_address())? addr : Log_Addr(false); }
        void detach(const Chunk & chunk) {}
        void detach(const Chunk & chunk, Log_Addr addr) {}

        Phy_Addr physical(Log_Addr addr) { return addr; }
    };

    // DMA_Buffer (straightforward without paging)
    class DMA_Buffer: public Chunk
    {
    public:
        DMA_Buffer(unsigned int s): Chunk(s, Flags::CT) {
            db<MMU>(TRC) << "MMU::DMA_Buffer() => " << *this << endl;
        }

        Log_Addr log_address() const { return phy_address(); }

        friend Debug & operator<<(Debug & db, const DMA_Buffer & b) {
            db << "{phy=" << b.phy_address()
               << ",log=" << b.log_address()
               << ",size=" << b.size()
               << ",flags=" << b.flags() << "}";
            return db;
        }
    };


    // Class Translation performs manual logical to physical address translations for debugging purposes only
    class Translation {
        public:
            Translation(unsigned int test) {};
            Translation(EPOS::S::U::ELF* test) {};
    };

public:
    MMU() {}

    static Phy_Addr alloc(unsigned int bytes = 1) {
        Phy_Addr phy(false);
        if(bytes) {
            List::Element * e = _free.search_decrementing(bytes);
            if(e)
                phy = reinterpret_cast<unsigned int *>(e->object()) + e->size();
            else
                db<MMU>(ERR) << "MMU::alloc() failed!" << endl;
        }
        db<MMU>(TRC) << "MMU::alloc(bytes=" << bytes << ") => " << phy << endl;

        return phy;
    };

    static Phy_Addr calloc(unsigned int bytes = 1) {
        Phy_Addr phy = alloc(bytes);
        memset(phy, 0, bytes);
        return phy;
    }

    static void free(Phy_Addr addr, unsigned int n = 1) {
        db<MMU>(TRC) << "MMU::free(addr=" << addr << ",n=" << n << ")" << endl;

        // No unaligned addresses if the CPU doesn't support it
        assert(Traits<CPU>::unaligned_memory_access || !(addr % 4));

        // Free blocks must be large enough to contain a list element
        assert(n > sizeof (List::Element));

        if(addr && n) {
            List::Element * e = new (addr) List::Element(addr, n);
            List::Element * m1, * m2;
            _free.insert_merging(e, &m1, &m2);
        }
    }

    static unsigned int allocable() { return _free.head() ? _free.head()->size() : 0; }

    static Page_Directory * volatile current() { return 0; }

    static Phy_Addr physical(Log_Addr addr) { return addr; }

    static PT_Entry phy2pte(Phy_Addr frame, Page_Flags flags);
    static Phy_Addr pte2phy(PT_Entry entry);
    static PD_Entry phy2pde(Phy_Addr frame);
    static Phy_Addr pde2phy(PD_Entry entry);


    static void flush_tlb() {}
    static void flush_tlb(Log_Addr addr) {}

    static Log_Addr phy2log(Phy_Addr phy) { return Log_Addr((MEM_BASE == PHY_MEM) ? phy : (MEM_BASE > PHY_MEM) ? phy - (MEM_BASE - PHY_MEM) : phy + (PHY_MEM - MEM_BASE)); }
    static Phy_Addr log2phy(Log_Addr log) { return Phy_Addr((MEM_BASE == PHY_MEM) ? log : (MEM_BASE > PHY_MEM) ? log + (MEM_BASE - PHY_MEM) : log - (PHY_MEM - MEM_BASE)); }

    static Color phy2color(Phy_Addr phy);

    static Color log2color(Log_Addr log);

private:
    static void init();

private:
    static List _free;
};

__END_SYS

#endif
