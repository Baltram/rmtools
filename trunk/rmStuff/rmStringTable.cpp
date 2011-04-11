#include "rmStringTable.h"

rmOut rmStringTable::out("rmStringTable.cpp");

rmStringTable::rmStringTable(rmU32 nStringsExpected)
{
	buffer.push_back(vector<rmChar>(1, '$')); // So 'buffer.back().front()' works.
	buffer.back().reserve(max<rmU32>(200, (nStringsExpected / 2)));

	buckets.resize(nStringsExpected * 2);
	memset(&buckets.front(), (-1), (buckets.size() * sizeof(rmU16)));

	blocks.reserve(nStringsExpected / 8);
}

void rmStringTable::reset(void)
{
	buffer.resize(1);
	buffer.back().resize(1);

	memset(&buckets.front(), (-1), (buckets.size() * sizeof(rmU16)));

	blocks.clear();
}

rmU32 rmStringTable::count(void) const
{
	return blocks.size();
}

MY_INLINE
rmBool rmStringTable::equal(rmLPCChar s, rmU32 size, rmU32 hash, const Block & b) const
{
	if (b.hash == hash) {
		rmU16 size2 = *(rmU16 *)(b.data);
		if (size2 == size) {
			if (strncmp((b.data + 2), s, size) == 0) {
				return true;
			}
		}
	}
	return false;
}

MY_INLINE
rmBool rmStringTable::findString(rmLPCChar s, rmU32 size, const rmU16 *& bucket, rmU32 hash) const
{
	bucket = &buckets[hash % (buckets.size() - 1)] + 1;
	if (*bucket == static_cast<rmU16>(-1)) {
		return false;
	}
	const Block & b = blocks[*bucket];
	if (equal(s, size, hash, b)) {
		return true;
	}
	// Collision.
	//OUT_MSG("col: " << string(s, size) << " " << string((b.data + 2), *(rmU16 *)b.data))
	if ((bucket - &buckets.front()) < 31) {
		bucket += 30; // Min. 30 alternative buckets.
	}
	while(*(--bucket) != static_cast<rmU16>(-1)) {
		const Block & b2 = blocks[*bucket];
		if (equal(s, size, hash, b2)) {
			return true;
		}
	}
	return false;
}

void rmStringTable::rehash(void)
{
	buckets.resize(buckets.size() * 3 / 2);
	memset(&buckets.front(), (-1), (buckets.size() * sizeof(rmU16)));
	rmLPCChar s = "$p";
	rmU32 size = 2;
	for (rmU32 i = 0, ie = blocks.size(); i != ie; ++i) {
		rmU32 hash = blocks[i].hash;
		const rmU16 * bucket;

		findString(s, size, bucket, hash); // 'bucket' points now to an empty bucket;
		if (bucket == &buckets.front()) { // Should not happen.
			rehash();
			return;
		}

		*const_cast<rmU16 *>(bucket) = i;
	}
}

rmU16 rmStringTable::getIndex(const string & str) const
{
	rmLPCChar s = str.data();
	rmU32 size = str.size();

	rmU32 hash;
	MurmurHash(s, size, &hash);
	const rmU16 * bucket;

	if (findString(s, size, bucket, hash)) {
		return *bucket;
	}
	return (-1);
}

string rmStringTable::getString(rmU16 index) const
{
	if (index >= blocks.size()) {
		OUT_WARN("Got too large index")
		return "$p";
	}
	rmChar * data = blocks[index].data;
	return string((data + 2), (*(rmU16 *)(data)));
}

rmU16 rmStringTable::registerString(const string & str)
{
	rmLPCChar s = str.data();
	rmU32 size = str.size();

	rmU32 hash;
	MurmurHash(s, size, &hash);
	const rmU16 * bucket;

	if (findString(s, size, bucket, hash)) {
		return *bucket;
	}

	// Resize if no alternative empty bucket was found or if the load factor is greater than 2/3.
	if ((bucket == &buckets.front()) || ((blocks.size() * 3) > (buckets.size() * 2))) {
		rehash();
		return registerString(s);
	}

	vector<rmChar> * buf = &buffer.back();
	// Ensure there is enough space in 'buf'.
	if ((buf->capacity() - buf->size()) < (size + 2 + 10)) {
		buffer.push_back(vector<rmChar>(1, '$'));
		rmU32 newSize = (buf->capacity() * 3 / 2);
		buf = &buffer.back();
		buf->reserve(newSize);
	}

	*const_cast<rmU16 *>(bucket) = blocks.size();
	Block b;
	b.data = (&buf->back() + 1);
	b.hash = hash;
	blocks.push_back(b);

	buf->resize(buf->size() + size + 2);
	*(rmU16 *)(b.data) = size;
	memcpy((b.data + 2), s, size);
	return *bucket;
}

void rmStringTable::readInStringList(rmInFileStream & src)
{
	reset();
	src.skip(5);
	rmU32 nStrings = src.readLong();
	rmU32 tableSize = nStrings * sizeof(rmU16);

	buckets.resize(nStrings * 2 * 2 / 3); // So it's size is (nStrings * 2) after rehashing.
	blocks.reserve(nStrings * 5 / 4 + 20); // 'nStrings' would be enough here but maybe some further strings will be inserted later.
	blocks.resize(nStrings);

	for (rmU32 i = 0; i != nStrings; ++i) {
		rmU32 stringSize = src.readShort();
		src.skip(stringSize);
		tableSize += stringSize;
	}
	src.skip(tableSize * (-1));

	vector<rmChar> & buf = buffer.back();
	buf.resize(tableSize + 1);
	rmChar * data = &buf[1];
	src.read(data, tableSize);

	for (rmU32 i = 0; i != nStrings; ++i) {
		blocks[i].data = data;
		MurmurHash((data + 2), *(rmU16 *)(data), &blocks[i].hash);
		data += (*(rmU16 *)(data) + sizeof(rmU16));
	}

	rehash();
}

void rmStringTable::writeStringList(rmOutFileStream & dest) const
{
	dest.writeLong(-559038737); // EFBEADDE.
	dest.writeByte(1); // Version.
	dest.writeLong(blocks.size());

	for (list< vector<rmChar> >::const_iterator i = buffer.begin(), ie = buffer.end(); i != ie; ++i) {
		if (i->size() < 2) {
			continue;
		}
		dest.write((&(*i)[1]), (i->size() - 1));
	}
}
