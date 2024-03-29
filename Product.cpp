#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include "Product.h"
using namespace std;

namespace AMA
{

	Product::Product(char type)
	{
		m_type = type;
		m_sku[0] = '\0';
		m_unit[0] = '\0';
		m_name = nullptr;
		m_qty = 0;
		m_needed = 0;
		m_price = 0.0;
		m_taxable = false;
	}

	Product::Product(const char * sku, const char * name, const char * unit, int quantity, bool taxable, double price, int needed)
	{
		bool notNull = sku != nullptr && name != nullptr && unit != nullptr;

		if (notNull)
		{
			strncpy(m_sku, sku, strlen(sku));
			m_sku[strlen(sku)] = '\0';

			Product::name(name);

			strncpy(m_unit, unit, strlen(unit));
			m_unit[strlen(unit)] = '\0';
		}

		m_qty = quantity;
		m_taxable = taxable;
		m_price = price;
		m_needed = needed;
	}

	Product::Product(const Product & other)
	{
		*this = other;
	}

	Product & Product::operator=(const Product & other)
	{
		if (this != &other)
		{
			int sku_l = strlen(other.m_sku);
			int unit_l = strlen(other.m_unit);

			m_type = other.m_type;
			m_qty = other.m_qty;
			m_needed = other.m_needed;
			m_price = other.m_price;
			m_taxable = other.m_taxable;

			name(other.m_name);

			strncpy(m_sku, other.m_sku, sku_l);
			m_sku[sku_l] = '\0';

			strncpy(m_unit, other.m_unit, unit_l);
			m_unit[unit_l] = '\0';
		}

		return *this;
	}

	Product::~Product()
	{
		delete[] m_name;
	}
	std::fstream & Product::store(std::fstream & file, bool newLine) const
	{
		file << m_type << ',' << m_sku << ',' << m_name << ',' << m_unit << ',' << m_taxable << ',' << m_price << ',' << m_qty << ',' << m_needed;
		if (newLine)
			file << endl;
		return file;
	}
	std::fstream & Product::load(std::fstream & file)
	{
		char sku_[MAX_CHAR_SKU];
		char name_[MAX_CHAR_NAME];
		char unit_[MAX_CHAR_UNIT];
		double price;
		int qty, needed;
		char tax;
		bool taxable;

		if (file.is_open())
		{
			file.getline(sku_, MAX_CHAR_SKU, ',');
			sku_[strlen(sku_)] = '\0';

			file.getline(name_, MAX_CHAR_NAME, ',');
			name_[strlen(name_)] = '\0';

			file.getline(unit_, MAX_CHAR_UNIT, ',');
			unit_[strlen(unit_)] = '\0';

			file >> tax;

			if (tax == '1')
				taxable = true;
			else if (tax == '0')
				taxable = false;

			file.ignore();

			file >> price;
			file.ignore();

			file >> qty;
			file.ignore();

			file >> needed;
			file.ignore();

			*this = Product(sku_, name_, unit_, qty, taxable, price, needed);
		}

		return file;
	}
	std::ostream & Product::write(std::ostream & os, bool linear) const
	{
		if (!(er.isClear()))
		{
			os << er.message();
		}

		else if (linear)
		{
			os << setw(MAX_CHAR_SKU) << left << m_sku << '|'
				<< setw(20) << left << m_name << '|'
				<< setw(7) << right << fixed << setprecision(2) << cost() << '|'
				<< setw(4) << right << m_qty << '|'
				<< setw(10) << left << m_unit << '|'
				<< setw(4) << right << m_needed << '|';
		}
		else
		{
			os << " Sku: " << m_sku << endl
				<< " Name (no spaces): " << m_name << endl
				<< " Price: " << m_price << endl;

			if (m_taxable)
				os << " Price after tax: " << cost() << endl;
			else
			{
				os << " Price after tax: N/A" << endl;
			}

			os << " Quantity on Hand: " << m_qty << ' ' << m_unit << endl
				<< " Quantity needed: " << m_needed;
		}

		return os;
	}
	std::istream & Product::read(std::istream & is)
	{
		char taxed;
		char * address = new char[MAX_CHAR_NAME + 1];
		int qty, need;
		double _price;

		if (!is.fail())
		{
			cout << " Sku: ";
			is >> m_sku;
			cin.ignore();

			cout << " Name (no spaces): ";
			is >> address;
			name(address);

			cout << " Unit: ";
			is >> m_unit;

			cout << " Taxed? (y/n): ";
			is >> taxed;

			if (!is.fail())
			{
				er.clear();

				if (taxed)
				{
					bool y_valid = taxed == 'y' || taxed == 'Y';
					bool n_valid = taxed == 'n' || taxed == 'N';

					if (y_valid)
						m_taxable = true;

					if (n_valid)
						m_taxable = false;

					if (!(y_valid || n_valid))
					{
						is.setstate(std::ios::failbit);
						er.message("Only (Y)es or (N)o are acceptable");
						return is;
					}
				}
			}
			else
			{
				is.setstate(std::ios::failbit);
				er.message("Only (Y)es or (N)o are acceptable");
				return is;
			}

			cout << " Price: ";
			is >> _price;

			if (is.fail())
			{
				er.clear();
				is.setstate(ios::failbit);
				er.message("Invalid Price Entry");
				return is;
			}
			else
				price(_price);

			cout << " Quantity on hand: ";
			is >> qty;


			if (is.fail())
			{
				er.clear();
				er.message("Invalid Quantity Entry");
				is.setstate(ios::failbit);
				return is;
			}
			else
				quantity(qty);

			cout << " Quantity needed: ";
			is >> need;
			cin.ignore();

			if (is.fail())
			{
				er.clear();
				er.message("Invalid Quantity Needed Entry");
				is.setstate(ios::failbit);
				return is;
			}
			else
				needed(need);

			if (!is.fail())
			{
				er.clear();
			}
		}

		return is;
	}
	bool Product::operator==(const char * rhs) const
	{
		if (rhs == sku())
			return true;
		else
			return false;
	}
	double Product::total_cost() const
	{
		return m_qty * cost();
	}
	void Product::quantity(int quantity)
	{
		m_qty = quantity;
	}
	void Product::price(double price)
	{
		m_price = price;
	}
	void Product::needed(int needed)
	{
		m_needed = needed;
	}
	bool Product::isEmpty() const
	{
		if (m_name == nullptr)
			return true;
		else
			return false;
	}
	char Product::type() const
	{
		return m_type;
	}
	int Product::qtyNeeded() const
	{
		return m_needed;
	}
	int Product::quantity() const
	{
		return m_qty;
	}
	bool Product::operator>(const char * rhs) const
	{
		if (strcmp(m_sku, rhs) > 0)
			return true;
		else
			return false;
	}
	bool Product::operator>(const iProduct & rhs) const
	{
		if (strcmp(m_name, rhs.name()) > 0)
			return true;
		else
			return false;
	}
	int Product::operator+=(int units)
	{
		if (units > 0)
		{
			m_qty += units;
			return m_qty;
		}
		else
			return m_qty;
	}
	void Product::name(const char * name)
	{
		if (name != nullptr)
		{
			int length = strlen(name);

			m_name = new char[length + 1];
			for (int i = 0; i < length; i++)
			{
				m_name[i] = name[i];
			}
			m_name[length] = '\0';
		}
		else if (name == nullptr)
		{
			delete[] m_name;
			m_name = nullptr;
		}
	}

	const char * Product::name() const
	{
		if (m_name != nullptr)
			return m_name;
		else
			return nullptr;
	}
	const char * Product::sku() const
	{
		return m_sku;
	}
	const char * Product::unit() const
	{
		return m_unit;
	}
	bool Product::taxed() const
	{
		return m_taxable;
	}
	double Product::price() const
	{
		return m_price;
	}
	double Product::cost() const
	{
		if (m_taxable)
			return price() * (TAX_RATE + 1);
		else
			return price();
	}
	void Product::message(const char * errorMessage)
	{
		if (errorMessage != nullptr)
			er.message(errorMessage);
	}
	bool Product::isClear() const
	{
		return er.isClear();
	}
	std::ostream& operator<<(std::ostream& os, const iProduct& other)
	{
		return other.write(os, true);
	}
	std::istream& operator>>(std::istream& is, iProduct& other)
	{
		return other.read(is);
	}
}
