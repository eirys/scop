/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ppm_loader.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: etran <etran@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/12 13:57:01 by eli               #+#    #+#             */
/*   Updated: 2023/05/13 02:30:34 by etran            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PPM_LOADER_HPP
# define PPM_LOADER_HPP

// Std
# include <string>
# include <fstream>

# include "image_loader.hpp"
# include "image_handler.hpp"

namespace scop {

enum FormatPPM {
	P3 = 0,		// ASCII
	P6 = 1		// BINARY
};

/**
 * PPM files parser.
*/
class PpmLoader: public ImageLoader {
public:
	/* ========================================================================= */
	/*                                  METHODS                                  */
	/* ========================================================================= */

	PpmLoader(const std::string& path);

	~PpmLoader() = default;

	PpmLoader() = delete;
	PpmLoader(const PpmLoader& x) = delete;
	PpmLoader(PpmLoader&& x) = delete;
	PpmLoader&	operator=(const PpmLoader& x) = delete;

	/* ========================================================================= */

	scop::Image		load() override;

private:
	/* ========================================================================= */
	/*                                  TYPEDEFS                                 */
	/* ========================================================================= */

	typedef		ImageLoader				base;
	typedef		std::vector<uint32_t>	Pixels;
	typedef		enum FormatPPM			Format;
	typedef		Pixels (PpmLoader::*ParseBodyFn)();

	/* ========================================================================= */
	/*                               CLASS MEMBERS                               */
	/* ========================================================================= */

	Format		format;
	uint8_t		max_color;
	size_t		cursor = 0;	// Used to parse the file
	size_t		line = 1;

	/* ========================================================================= */

	void		parseHeader();
	Pixels		parseBodyP3();
	Pixels		parseBodyP6();

	Format		expectFormat();
	uint32_t	expectNumber();

	bool		skipWhitespace() noexcept;
	bool		skipComment() noexcept;
	void		ignoreChunk() noexcept;

	/* ========================================================================= */
	/*                                 EXCEPTION                                 */
	/* ========================================================================= */

	class PpmParseError: public std::exception {
	public:
		PpmParseError() = delete;
		PpmParseError(const std::string& spec):
			spec(spec) {}

		const char*	what() const noexcept override {
			return spec.c_str();
		}

	private:
		const std::string	spec;
	};

}; // class PpmLoader

} // namespace scop
#endif